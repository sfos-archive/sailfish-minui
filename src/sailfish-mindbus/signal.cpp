/****************************************************************************
**
** Copyright (C) 2017 Jolla Ltd.
** Contact: Andrew den Exter <andrew.den.exter@jolla.com>
**
****************************************************************************/

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
