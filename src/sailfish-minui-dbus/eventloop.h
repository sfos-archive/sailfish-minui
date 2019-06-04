/****************************************************************************
**
** Copyright (C) 2017 Jolla Ltd.
** Contact: Andrew den Exter <andrew.den.exter@jolla.com>
**
****************************************************************************/

#ifndef SAILFISH_MINUI_DBUS_EVENTLOOP_H
#define SAILFISH_MINUI_DBUS_EVENTLOOP_H

#include <sailfish-minui/eventloop.h>

#include <sailfish-mindbus/connection.h>

namespace Sailfish { namespace MinUi { namespace DBus {

class EventLoop : public Sailfish::MinUi::EventLoop
{
public:
    EventLoop();
    ~EventLoop();

protected:
    void timerExpired(void *data) override;

    bool notify(int descriptor, uint32_t events, void *data, void *callback = nullptr) override;
    bool dispatch() override;

private:
    inline static dbus_bool_t add_dbus_watch(DBusWatch *watch, void *data);
    inline static void remove_dbus_watch(DBusWatch *watch, void *data);

    inline static dbus_bool_t add_dbus_timeout(DBusTimeout *timeout, void *data);
    inline static void remove_dbus_timeout(DBusTimeout *timeout, void *data);
    inline static void toggle_dbus_timeout(DBusTimeout *timeout, void *data);

    inline static void update_dbus_dispatch_status(DBusConnection *connection, DBusDispatchStatus status, void *data);

    Sailfish::MinDBus::Connection m_systemBus;
    DBusDispatchStatus m_dispatchStatus;
};

}}}

#endif
