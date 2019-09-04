/*
 * Copyright (C) 2017-2019 Jolla Ltd.
 *
 * License: Proprietary
 */

#ifndef SAILFISH_MINUI_EVENTLOOP_H
#define SAILFISH_MINUI_EVENTLOOP_H

#include <vector>
#include <functional>

typedef bool (NotifierCallbackType)(int descriptor, uint32_t events);

namespace Sailfish { namespace MinUi {

class Window;

class EventLoop
{
public:
    EventLoop();
    ~EventLoop();

    static EventLoop *instance();

    int execute();

    void exit(int result = EXIT_SUCCESS);

    int createTimer(int interval, const std::function<void()> &callback);
    void cancelTimer(int id);

    void singleShot(const std::function<void()> &callback);

    void onTerminated(const std::function<void()> &callback);

    bool addNotifierCallback(int descriptor, const std::function<NotifierCallbackType> &callback);
    void removeNotifier(int descriptor);

protected:
    void createTimer(int interval, void *data);
    void cancelTimer(void *data);

    virtual void timerExpired(void *data);

    bool addNotifier(int descriptor, void *data, const std::function<NotifierCallbackType> &callback = nullptr);

    virtual bool notify(int descriptor, uint32_t events, void *data);
    virtual bool dispatch();

private:
    friend class Window;

    struct Notifier {
        int fd;
        void *data;
        std::function<NotifierCallbackType> callback;
    };

    struct Timer {
        Timer() = default;
        Timer(const Timer &) = default;
        Timer(int id, int interval, int64_t expiration, const std::function<void()> &callback)
            : expiration(expiration), callback(callback), data(nullptr), interval(interval), id(id) {}
        Timer(int interval, int64_t expiration, void *data)
            : expiration(expiration), data(data), interval(interval), id(-1) {}
        int64_t expiration;
        std::function<void()> callback;
        void *data;
        int interval;
        int id;
    };

    inline void insertTimer(const Timer &timer);
    inline void setWindow(Window *window);

    static inline int ev_input_callback(int fd, uint32_t epevents, void *data);
    static inline int ev_notifier_callback(int fd, uint32_t epevents, void *data);
    static inline int terminated_callback(int fd, uint32_t epevents, void *data);

    std::vector<Notifier> m_notifiers;
    std::vector<Timer> m_timers;
    std::vector<std::function<void()>> m_singleShots;
    std::function<void()> m_terminated;
    Window *m_window;
    int m_result;
    bool m_executing;
};

}}

#endif
