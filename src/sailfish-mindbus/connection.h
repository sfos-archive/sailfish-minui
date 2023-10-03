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
