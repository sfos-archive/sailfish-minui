/*
 * Copyright (C) 2017-2019 Jolla Ltd.
 *
 * License: Proprietary
 */

#include "object.h"

#include <algorithm>
#include <string.h>

namespace Sailfish { namespace MinDBus {


class PendingGetProperty : public AbstractPendingCall
{
public:
    PendingGetProperty(DBusPendingCall *call, PropertySubscription *subscription);

protected:
    void notify(DBusMessage *message) override;

private:
    PropertySubscription * const m_subscription;
};

Object::Object(const Connection &connection, const char *service, const char *interface)
    : m_connection(connection)
    , m_service(service)
    , m_interface(interface)
{
}

Object::Object(const Connection &connection, const char *service, const char *path, const char *interface)
    : m_connection(connection)
    , m_path(path)
    , m_service(service)
    , m_interface(interface)
{
}

Object::~Object()
{
    reset();
}

const char *Object::service() const
{
    return m_service;
}

const char *Object::interface() const
{
    return m_interface;
}

const char *Object::path()
{
    return m_path.data();
}

void Object::setPath(const char *path)
{
    reset();

    m_path = path;
}

void Object::disconnectSignals()
{
    auto handlers = m_signals;
    for (auto handler : handlers) {
        handler->disconnect();
    }
}


void Object::unsubscribeProperties()
{
    auto subscriptions = m_subscriptions;
    for (auto subscription : subscriptions) {
        subscription->unsubscribe();
    }
}

void Object::reset()
{
    disconnectSignals();
    unsubscribeProperties();
}

PropertySubscription *Object::subscribeToProperty(
        const char *name, const std::function<void(DBusMessageIter *)> &notify)
{
    if (m_subscriptions.empty()) {
        const char *ruleFormat = "type='signal', interface='org.freedesktop.DBus.Properties', member='PropertiesChanged', path='%s'";
        int length = std::snprintf(nullptr, 0, ruleFormat, m_path.data());
        m_rule.resize(length + 1);;
        std::snprintf(&m_rule[0], m_rule.length(), ruleFormat, m_path.data());

        dbus_bus_add_match(m_connection, m_rule.data(), nullptr);

        dbus_connection_add_filter(m_connection, property_change_callback, this, nullptr);
    }
    auto subscription = new PropertySubscription(this, name, notify);
    m_subscriptions.push_back(subscription);
    return subscription;
}

void Object::unsubscribe(PropertySubscription *subscription)
{
    if (!m_subscriptions.empty()) {
        m_subscriptions.erase(std::remove(m_subscriptions.begin(), m_subscriptions.end(), subscription));

        if (m_subscriptions.empty()) {
            dbus_connection_remove_filter(m_connection, property_change_callback, this);

            dbus_bus_remove_match(m_connection, m_rule.data(), nullptr);
        }
    }
}

DBusHandlerResult Object::property_change_callback(
        DBusConnection *, DBusMessage *message, void *data)
{
    const auto object = static_cast<Object *>(data);

    if (dbus_message_is_signal(message, "org.freedesktop.DBus.Properties", "PropertiesChanged")
            && dbus_message_get_path(message) == object->m_path) {
        DBusMessageIter iterator;
        const char *interface = nullptr;
        if (dbus_message_iter_init(message, &iterator)
                && dbus_message_iter_get_arg_type(&iterator) == DBUS_TYPE_STRING
                && (dbus_message_iter_get_basic(&iterator, &interface), true) // returns void, don't let that interrupt the flow.
                && (strcmp(interface, object->m_interface) == 0)) {
            dbus_message_iter_next(&iterator);

            // changed_properties.
            if (dbus_message_iter_get_arg_type(&iterator) == DBUS_TYPE_ARRAY) {
                DBusMessageIter array;
                dbus_message_iter_recurse(&iterator, &array);
                while (dbus_message_iter_get_arg_type(&array) == DBUS_TYPE_DICT_ENTRY) {
                    DBusMessageIter entry;
                    dbus_message_iter_recurse(&array, &entry);
                    dbus_message_iter_next(&array);

                    if (dbus_message_iter_get_arg_type(&entry) != DBUS_TYPE_STRING) {
                        continue;
                    }

                    const char *propertyName;
                    dbus_message_iter_get_basic(&entry, &propertyName);
                    for (PropertySubscription *subscription : object->m_subscriptions) {
                        if (subscription->m_name == propertyName) {
                            dbus_message_iter_next(&entry);

                            if (dbus_message_iter_get_arg_type(&entry) == DBUS_TYPE_VARIANT) {
                                DBusMessageIter value;
                                dbus_message_iter_recurse(&entry, &value);

                                subscription->m_notify(&value);

                                // Only support one handler for a property, otherwise we'll have to deal
                                // with the possiblity someone may edit the list mid iteration.
                                break;
                            }
                        }
                    }
                }
            }
            dbus_message_iter_next(&iterator);

            // invalidated_properties.
            if (dbus_message_iter_get_arg_type(&iterator) == DBUS_TYPE_ARRAY) {
                DBusMessageIter array;
                dbus_message_iter_recurse(&iterator, &array);
                while (dbus_message_iter_get_arg_type(&array) == DBUS_TYPE_STRING) {
                    const char *propertyName;
                    dbus_message_iter_get_basic(&array, &propertyName);
                    dbus_message_iter_next(&array);

                    for (PropertySubscription *subscription : object->m_subscriptions) {
                        if (subscription->m_name == propertyName) {
                            subscription->update();
                        }
                    }
                }
            }

            return DBUS_HANDLER_RESULT_HANDLED;
        }
    }

    return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
}

PropertySubscription::PropertySubscription(
        Object *object, const char *name, const std::function<void(DBusMessageIter *)> &notify)
    : m_name(name)
    , m_notify(notify)
    , m_object(object)
    , m_query(nullptr)
{
    if (m_query) {
        m_query->cancel();
    }
}

PropertySubscription::~PropertySubscription()
{
}

void PropertySubscription::update()
{
    if (m_query) {
        return;
    }
    Message message = createMethodCall(
                m_object->service(),
                m_object->path(),
                "org.freedesktop.DBus.Properties",
                "Get",
                m_name.data());
    DBusPendingCall *call = nullptr;
    m_query = dbus_connection_send_with_reply(m_object->m_connection, message, &call, -1)
            ? new PendingGetProperty(call, this)
            : nullptr;

}

void PropertySubscription::unsubscribe()
{
    m_object->unsubscribe(this);
    delete this;
}

PendingGetProperty::PendingGetProperty(DBusPendingCall *call, PropertySubscription *subscription)
    : AbstractPendingCall(call)
    , m_subscription(subscription)
{
}

void PendingGetProperty::notify(DBusMessage *message)
{
    m_subscription->m_query = nullptr;

    DBusMessageIter iterator;
    if (dbus_message_iter_init(message, &iterator)) {
        m_subscription->m_notify(&iterator);
    }
}


}}
