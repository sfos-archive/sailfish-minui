/****************************************************************************
**
** Copyright (C) 2017 Jolla Ltd.
** Contact: Andrew den Exter <andrew.den.exter@jolla.com>
**
****************************************************************************/

#ifndef SAILFISH_MINDBUS_CONNECTION_H
#define SAILFISH_MINDBUS_CONNECTION_H

#include <sailfish-mindbus/pendingcall.h>

#include <dbus/dbus.h>

namespace Sailfish { namespace MinDBus {

typedef Pointer<DBusConnection> Connection;

template <> inline void Pointer<DBusConnection>::reference(DBusConnection *pointer)
{
    dbus_connection_ref(pointer);
}

template <> inline void Pointer<DBusConnection>::release(DBusConnection *pointer)
{
    dbus_connection_unref(pointer);
}

Connection systemBus();

template <typename... Return, typename... Arguments> PendingCall<Return...> *callMethod(
        DBusConnection *connection,
        const char *service,
        const char *path,
        const char *interface,
        const char *name,
        Arguments... arguments)
{
    Message message = createMethodCall(service, path, interface, name, arguments...);

    DBusPendingCall *call = nullptr;
    return dbus_connection_send_with_reply(connection, message, &call, -1)
            ? new PendingCall<Return...>(call)
            : nullptr;
}

}}

#endif
