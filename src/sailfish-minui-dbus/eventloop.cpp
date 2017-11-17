/****************************************************************************
**
** Copyright (C) 2017 Jolla Ltd.
** Contact: Andrew den Exter <andrew.den.exter@jolla.com>
**
****************************************************************************/

#include "eventloop.h"

#include <sys/epoll.h>

#include <iostream>

namespace Sailfish { namespace MinUi { namespace DBus {

EventLoop::EventLoop()
    : m_dispatchStatus(DBUS_DISPATCH_COMPLETE)
{
    dbus_threads_init_default();

    m_systemBus = Sailfish::MinDBus::systemBus();

    dbus_connection_set_watch_functions(
                m_systemBus, add_dbus_watch, remove_dbus_watch, nullptr, this, nullptr);
    dbus_connection_set_timeout_functions(
                m_systemBus, add_dbus_timeout, remove_dbus_timeout, toggle_dbus_timeout, this, nullptr);
    dbus_connection_set_dispatch_status_function(
                m_systemBus, update_dbus_dispatch_status, this, nullptr);
}

EventLoop::~EventLoop()
{
}

void EventLoop::timerExpired(void *data)
{
    dbus_timeout_handle(static_cast<DBusTimeout *>(data));
}

bool EventLoop::notify(int, uint32_t events, void *data)
{
    DBusWatch *watch = static_cast<DBusWatch *>(data);

    if (dbus_watch_get_enabled(watch)) {
        int flags = 0;
        if (events & EPOLLIN) {
            flags |= DBUS_WATCH_READABLE;
        }
        if (events & EPOLLOUT) {
            flags |= DBUS_WATCH_WRITABLE;
        }
        if (dbus_watch_handle(watch, flags)) {
            m_dispatchStatus = dbus_connection_get_dispatch_status(m_systemBus);
            return true;
        } else {
            return false;
        }
    } else {
        return false;
    }
}

bool EventLoop::dispatch()
{
    if (m_dispatchStatus == DBUS_DISPATCH_DATA_REMAINS) {
        dbus_connection_dispatch(m_systemBus);
        m_dispatchStatus = dbus_connection_get_dispatch_status(m_systemBus);
        return true;
    } else {
        return false;
    }
}

dbus_bool_t EventLoop::add_dbus_watch(DBusWatch *watch, void *data)
{
    return static_cast<EventLoop *>(data)->addNotifier(dbus_watch_get_unix_fd(watch), watch)
            ? TRUE
            : FALSE;
}

void EventLoop::remove_dbus_watch(DBusWatch *watch, void *data)
{
    static_cast<EventLoop *>(data)->removeNotifier(dbus_watch_get_unix_fd(watch));
}

dbus_bool_t EventLoop::add_dbus_timeout(DBusTimeout *timeout, void *data)
{
    static_cast<EventLoop *>(data)->createTimer(dbus_timeout_get_interval(timeout), timeout);

    return TRUE;
}

void EventLoop::remove_dbus_timeout(DBusTimeout *timeout, void *data)
{
    static_cast<EventLoop *>(data)->cancelTimer(timeout);
}

void EventLoop::toggle_dbus_timeout(DBusTimeout *timeout, void *data)
{
    if (dbus_timeout_get_enabled(timeout)) {
        add_dbus_timeout(timeout, data);
    } else {
        remove_dbus_timeout(timeout, data);
    }
}

void EventLoop::update_dbus_dispatch_status(
        DBusConnection *connection, DBusDispatchStatus status, void *data)
{
    const auto loop = static_cast<EventLoop *>(data);

    if (connection == loop->m_systemBus) {
        static_cast<EventLoop *>(data)->m_dispatchStatus = status;
    }
}

}}}
