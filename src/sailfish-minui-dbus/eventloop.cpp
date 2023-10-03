/****************************************************************************************
** Copyright (c) 2017 - 2023 Jolla Ltd.
**
** All rights reserved.
**
** This file is part of Sailfish Minui package.
**
** You may use this file under the terms of BSD license as follows:
**
** Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are met:
**
** 1. Redistributions of source code must retain the above copyright notice, this
**    list of conditions and the following disclaimer.
**
** 2. Redistributions in binary form must reproduce the above copyright notice,
**    this list of conditions and the following disclaimer in the documentation
**    and/or other materials provided with the distribution.
**
** 3. Neither the name of the copyright holder nor the names of its
**    contributors may be used to endorse or promote products derived from
**    this software without specific prior written permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
** AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
** IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
** DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
** FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
** DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
** SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
** CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
** OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
**
****************************************************************************************/

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


    /* While dispatch status starts off as DBUS_DISPATCH_COMPLETE,
     * it might have changed during connect and we are not going
     * to get notifications about changes that took place before
     * installing the callbacks -> Get back in sync with libdbus.
     */
    m_dispatchStatus = dbus_connection_get_dispatch_status(m_systemBus);
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
            return true;
        }
    }
    return false;
}

bool EventLoop::dispatch()
{
    if (m_dispatchStatus == DBUS_DISPATCH_DATA_REMAINS)
        dbus_connection_dispatch(m_systemBus);

    /* Note: Cached state can change during dispatching */
    return m_dispatchStatus == DBUS_DISPATCH_DATA_REMAINS;
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
