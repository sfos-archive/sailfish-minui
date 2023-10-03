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
#include "signal.h"

#include "object.h"

#include <algorithm>

namespace Sailfish { namespace MinDBus {

AbstractSignalHandler::AbstractSignalHandler(
        DBusConnection *connection, const char *path, const char *interface, const char *name)
    : m_path(path)
    , m_connection(connection)
    , m_interface(interface)
    , m_name(name)
{
    const char *ruleFormat = "type='signal', interface='%s', member='%s', path='%s'";
    int length = std::snprintf(nullptr, 0, ruleFormat, interface, name, path);
    m_rule.resize(length + 1);;
    std::snprintf(&m_rule[0], m_rule.length(), ruleFormat, interface, name, path);

    dbus_bus_add_match(m_connection, m_rule.data(), nullptr);

    dbus_connection_add_filter(connection, notify_callback, this, free_callback);
}

AbstractSignalHandler::~AbstractSignalHandler()
{
    dbus_bus_remove_match(m_connection, m_rule.data(), nullptr);
}

void AbstractSignalHandler::disconnect()
{
    if (m_object) {
        m_object->m_signals.erase(
                    std::remove(m_object->m_signals.begin(), m_object->m_signals.end(), this));
    }
    dbus_connection_remove_filter(m_connection, notify_callback, this);

    release();
}

void AbstractSignalHandler::release()
{
    if (--m_ref == 0) {
        delete this;
    }
}


DBusHandlerResult AbstractSignalHandler::notify_callback(DBusConnection *, DBusMessage *message, void *data)
{
    const auto handler = static_cast<AbstractSignalHandler *>(data);

    if (dbus_message_is_signal(message, handler->m_interface, handler->m_name)
            && dbus_message_get_path(message) == handler->m_path) {
        ++handler->m_ref;

        handler->notify(message);

        handler->release();

        return DBUS_HANDLER_RESULT_HANDLED;
    }

    return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
}

void AbstractSignalHandler::free_callback(void *data)
{
    static_cast<AbstractSignalHandler *>(data)->release();
}

}}
