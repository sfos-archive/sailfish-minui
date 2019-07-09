/****************************************************************************
**
** Copyright (C) 2017 Jolla Ltd.
** Contact: Andrew den Exter <andrew.den.exter@jolla.com>
**
****************************************************************************/

#include "eventloop.h"
#include "ui.h"

#include <algorithm>

#include <limits.h>
#include <signal.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include <linux/input.h>
#include <sys/epoll.h>
#include <sys/eventfd.h>

#include <cassert>

#include <iostream>

namespace Sailfish { namespace MinUi {

static int terminateSignalFd = -1;

static void terminateSignalHandler(int signal)
{
    int64_t eventData = signal;
    const ssize_t size = ::write(terminateSignalFd, &eventData, sizeof(eventData));
    assert(size == sizeof(eventData));
}

static int64_t currentTime()
{
    timespec time;
    clock_gettime(CLOCK_MONOTONIC, &time);

    return (time.tv_sec * INT64_C(1000)) + (time.tv_nsec / 1000000);
}

static EventLoop *globalEventLoop = nullptr;

/*!
    \class Sailfish::MinUi::EventLoop
    \brief An event loop for a MinUi application.

    Only one event loop can exist concurrently, this can be accessed through the \l instance()
    function.
*/

/*!
    Constructs a new event loop.
*/
EventLoop::EventLoop()
    : m_window(nullptr)
    , m_result(0)
    , m_executing(false)
{
    assert(!globalEventLoop);

    globalEventLoop = this;

    ev_init(ev_input_callback, this);

    assert(terminateSignalFd == -1);
    terminateSignalFd = ::eventfd(0, EFD_NONBLOCK);

    if (terminateSignalFd >= 0) {
        struct sigaction action;
        memset(&action, 0, sizeof(action));

        action.sa_handler = terminateSignalHandler;
        sigemptyset(&action.sa_mask);
        action.sa_flags |= SA_RESTART;

        sigaction(SIGTERM, &action, 0);

        ev_add_fd(terminateSignalFd, terminated_callback, this);
    }
}

/*!
    Destroys an event loop.
*/
EventLoop::~EventLoop()
{
    if (terminateSignalFd >= 0) {
        close(terminateSignalFd);
        terminateSignalFd = 0;
    }

    globalEventLoop = nullptr;
}

/*!
    Returns the current instance of the event loop.
*/
EventLoop *EventLoop::instance()
{
    return globalEventLoop;
}

/*!
    Executes the event loop. This will block, processing events until the \l exit() is called
    or the application is terminated at which point it will return the result code.
*/
int EventLoop::execute()
{
    if (m_executing) {
        return -1;
    }

    m_executing = true;

    int64_t timeout = -1;
    while (m_executing) {
        if (m_timers.size() > 0) {
            auto timer = m_timers.front();
            int64_t expires = timer.expiration - currentTime();

            if (expires <= 0) {
                // Move the timer to the next expiration point.
                m_timers.erase(m_timers.begin());

                timer.expiration += timer.interval;

                insertTimer(timer);

                if (timer.data) {
                    timerExpired(timer.data);
                } else if (timer.callback) {
                    timer.callback();
                }

                timeout = 0;
            } else if (timeout < 0 || timeout > expires) {
                timeout = expires;
            }
        }

        if (m_executing && m_singleShots.size() > 0) {
            auto singleShot = m_singleShots.front();
            m_singleShots.erase(m_singleShots.begin());

            singleShot();

            timeout = 0;
        }

        if (m_executing && ev_wait(std::min<int64_t>(timeout, INT_MAX)) == 0) {
            ev_dispatch();
        }

        timeout = -1;

        if (dispatch()) {
            timeout = 0;
        }
    }

    while (dispatch()) {}

    return m_result;
}

/*!
    Exits the event loop. The \l execute() function will return the \a result argument.
*/
void EventLoop::exit(int result)
{
    if (m_executing) {
        m_result = result;
        m_executing = false;
    }
}

/*!
    Creates a new timer which executes \a callback every \a interval milliseconds.

    Returns a unique ID for the timer which can be used to cancel the timer with \l cancelTimer().
*/
int EventLoop::createTimer(int interval, const std::function<void()> &callback)
{
    static int counter = 0;

    const int id = ++counter;

    insertTimer(Timer(id, interval, currentTime() + interval, callback));

    return id;
}

/*!
    Cancels a timer identified by \a id.
*/
void EventLoop::cancelTimer(int id)
{
    auto end = std::remove_if(m_timers.begin(), m_timers.end(), [id](const Timer &timer) {
        return timer.id == id;
    });
    m_timers.erase(end, m_timers.end());
}

/*!
    Creates a timer which will invoke \l timerExpired() passing \a data as an argument every
    \a interval milliseconds.
*/
void EventLoop::createTimer(int interval, void *data)
{
    insertTimer({ interval, currentTime() + interval, data });
}

/*!
    Cancels a timer which has created with \a data as an argument.
*/
void EventLoop::cancelTimer(void *data)
{
    auto end = std::remove_if(m_timers.begin(), m_timers.end(), [data](const Timer &timer) {
        return timer.data == data;
    });
    m_timers.erase(end, m_timers.end());
}

/*!
    Signals that a timer with the given context \a data has expired.
*/
void EventLoop::timerExpired(void *data)
{
    (void)data;
}

/*!
    Adds a callback type notifier for a socket \a descriptor which will call the given callback
    \a callback. Callback function should be type \l NotifierCallbackType bool(int descriptor, uint32_t events)

    Returns true if the notifier was successfully added.
*/
bool EventLoop::addNotifierCallback(int descriptor, const std::function<NotifierCallbackType> &callback)
{
    if (callback) {
        // Pass the address to the notifier
        return addNotifier(descriptor, nullptr, callback);
    } else {
        return false;
    }
}

/*!
    Adds a notifier for a socket \a descriptor which will call \l notify() with the given context
    \a data, if optional \a callback is defined it's used as the callback function address.

    Returns true if the notifier was successfully added.
*/
bool EventLoop::addNotifier(int descriptor, void *data, const std::function<NotifierCallbackType> &callback)
{
    // If a watch was removed and then re-added later or the fd was recycled restore the removed
    // watch instead of creating a new one.
    for (auto &notifier : m_notifiers) {
        if (notifier.fd == descriptor) {
            notifier.data = data;
            notifier.callback = callback;
            return true;
        }
    }

    if (ev_add_fd(descriptor, ev_notifier_callback, this) == 0) {
        m_notifiers.push_back({ descriptor, data, callback });
        return true;
    }

    return false;
}

/*!
    Removes a notifier for a socket \a descriptor.
*/
void EventLoop::removeNotifier(int descriptor)
{
    for (auto &notifier : m_notifiers) {
        if (notifier.fd == descriptor) {
            notifier.data = nullptr;
            notifier.callback = nullptr;
            break;
        }
    }
}

/*!
    Handles a notification of \a events for a socket \a descriptor with the given \a data.

    Returns true if the notification was handled successfully.
    */
bool EventLoop::notify(int descriptor, uint32_t events, void *data)
{
    (void)descriptor;
    (void)data;
    (void)events;

    return false;
}

/*!
    Dispatches further queued events.

    Returns true if the there are further events to process and false if there are not.
*/
bool EventLoop::dispatch()
{
    return false;
}

/*!
    Inserts a new \a timer.
*/
void EventLoop::insertTimer(const Timer &timer)
{
    auto position = std::find_if(m_timers.begin(), m_timers.end(), [&timer](const Timer &existing) {
        return timer.expiration < existing.expiration;
    });
    m_timers.insert(position, timer);
}

/*!
    Sets the active \a window.

    Input events will be delivered to this window.
*/
void EventLoop::setWindow(Window *window)
{
    m_window = window;
}

/*!
    Sets a \a callback which will be called when the application receives a SIGTERM signal.

    If no callback is set the event loop will exit immediately otherwise the it is up to the
    implementer of the callback to exit the application.
*/
void EventLoop::onTerminated(const std::function<void()> &callback)
{
    m_terminated = callback;
}

/*!
    Handles a input \a events for the file descriptor \a fd where \a data is a pointer to the
    event loop.

    Returns -1 if there was an error handling the events and 0 if it was handled successfully.
*/
int EventLoop::ev_input_callback(int fd, uint32_t events, void *data)
{
    EventLoop * const loop = static_cast<EventLoop *>(data);

    input_event event;

    if (ev_get_input(fd, events, &event) != 0) {
        return -1;
    }

    if (loop->m_window) {
        loop->m_window->inputEvent(fd, event);
    }

    return 0;
}

/*!
    Handles a notifier \a event for a file descriptor \a fd where \a data is a pointer to the
    event loop.

    Returns -1 if there was an error handling the events and 0 if it was handled successfully.
*/
int EventLoop::ev_notifier_callback(int fd, uint32_t event, void *data)
{
    const auto loop = static_cast<EventLoop *>(data);

    void *notifierData = nullptr;
    std::function<NotifierCallbackType> callback(nullptr);
    for (const auto &notifier : loop->m_notifiers) {
        if (notifier.fd == fd) {
            notifierData = notifier.data;
            callback = notifier.callback;
            break;
        }
    }

    if (callback) {
        callback(fd, event);
    }
    if (notifierData) {
        loop->notify(fd, event, notifierData);
    }

    return 0;
}

/*!
    Handles a SIGTERM \a event for a file descriptor \a fd where \a data is a pointer to the
    event loop.

    Returns -1 if there was an error handling the event and 0 if it was handled successfully.
*/
int EventLoop::terminated_callback(int fd, uint32_t event, void *data)
{
    (void)event;

    int64_t eventData;
    const ssize_t size = ::read(fd, &eventData, sizeof(eventData));
    assert(size == sizeof(eventData));

    const auto loop = static_cast<EventLoop *>(data);
    if (loop->m_terminated) {
        loop->m_terminated();
    } else {
        loop->exit(EXIT_FAILURE);
    }

    return 0;
}

}}
