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

EventLoop::EventLoop()
    : m_window(nullptr)
    , m_result(0)
    , m_executing(false)
{
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

EventLoop::~EventLoop()
{
    if (terminateSignalFd >= 0) {
        close(terminateSignalFd);
        terminateSignalFd = 0;
    }
}

int EventLoop::execute()
{
    if (m_executing) {
        return -1;
    }

    m_executing = true;

    int64_t timeout = -1;
    while (m_executing) {
        if (m_timers.size() > 0 && (timeout = m_timers.front().expiration - currentTime()) < 0) {
            // Move the timer to the next expiration point.
            auto timer = m_timers.front();
            m_timers.erase(m_timers.begin());

            timer.expiration += timer.interval;

            insertTimer(timer);

            if (timer.data) {
                timerExpired(timer.data);
            } else if (timer.callback) {
                timer.callback();
            }

            timeout = 0;
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

void EventLoop::exit(int result)
{
    if (m_executing) {
        m_result = result;
        m_executing = false;
    }
}

int EventLoop::createTimer(int interval, const std::function<void()> &callback)
{
    static int counter = 0;

    const int id = ++counter;

    insertTimer(Timer(id, interval, currentTime() + interval, callback));

    return id;
}

void EventLoop::cancelTimer(int id)
{
    auto end = std::remove_if(m_timers.begin(), m_timers.end(), [id](const Timer &timer) {
        return timer.id == id;
    });
    m_timers.erase(end, m_timers.end());
}

void EventLoop::createTimer(int interval, void *data)
{
    insertTimer({ interval, currentTime() + interval, data });
}

void EventLoop::cancelTimer(void *data)
{
    auto end = std::remove_if(m_timers.begin(), m_timers.end(), [data](const Timer &timer) {
        return timer.data == data;
    });
    m_timers.erase(end, m_timers.end());
}

void EventLoop::timerExpired(void *)
{
}

bool EventLoop::addNotifier(int descriptor, void *data)
{
    // If a watch was removed and then re-added later or the fd was recycled restore the removed
    // watch instead of creating a new one.
    for (auto &notifier : m_notifiers) {
        if (notifier.fd == descriptor) {
            notifier.data = data;
            return true;
        }
    }

    if (ev_add_fd(descriptor, ev_notifier_callback, this) == 0) {
        m_notifiers.push_back({ descriptor, data });
        return true;
    }

    return false;
}

void EventLoop::removeNotifier(int descriptor)
{
    for (auto &notifier : m_notifiers) {
        if (notifier.fd == descriptor) {
            notifier.data = nullptr;
            break;
        }
    }
}

bool EventLoop::notify(int, uint32_t, void *)
{
    return false;
}

bool EventLoop::dispatch()
{
    return false;
}

void EventLoop::insertTimer(const Timer &timer)
{
    auto position = std::find_if(m_timers.begin(), m_timers.end(), [&timer](const Timer &existing) {
        return timer.expiration < existing.expiration;
    });
    m_timers.insert(position, timer);
}

void EventLoop::setWindow(Window *window)
{
    m_window = window;
}

void EventLoop::onTerminated(const std::function<void()> &callback)
{
    m_terminated = callback;
}

int EventLoop::ev_input_callback(int fd, uint32_t epevents, void *data)
{
    EventLoop * const loop = static_cast<EventLoop *>(data);

    input_event event;

    if (ev_get_input(fd, epevents, &event) != 0) {
        return -1;
    }

    if (loop->m_window) {
        loop->m_window->inputEvent(fd, event);
    }

    return 0;
}

int EventLoop::ev_notifier_callback(int fd, uint32_t epevents, void *data)
{
    const auto loop = static_cast<EventLoop *>(data);

    void *notifierData = nullptr;
    for (const auto &notifier : loop->m_notifiers) {
        if (notifier.fd == fd) {
            notifierData = notifier.data;
            break;
        }
    }

    if (notifierData) {
        loop->notify(fd, epevents, notifierData);
    }

    return 0;
}

int EventLoop::terminated_callback(int fd, uint32_t, void *data)
{
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
