/*
 * Copyright (c) 2017-2019 Jolla Ltd.
 *
 * License: Proprietary
 */

#ifndef SAILFISH_MINDBUS_OBJECT_H
#define SAILFISH_MINDBUS_OBJECT_H

#include <sailfish-mindbus/connection.h>
#include <sailfish-mindbus/signal.h>

#include <string>
#include <vector>

namespace Sailfish { namespace MinDBus {

class PropertySubscription;
class Object
{
public:
    Object(const Connection &connection, const char *service, const char *interface);
    Object(const Connection &connection, const char *service, const char *path, const char *interface);
    ~Object();

    const char *service() const;
    const char *interface() const;

    const char *path();
    void setPath(const char *path);

    template <typename... Return, typename... Arguments> PendingCall<Return...> *call(
            const char *name,
            Arguments... arguments)
    {
        return callMethod<Return...>(
                    m_connection, m_service, m_path.data(), m_interface, name, arguments...);
    }

    template <typename... Arguments, typename Callback> AbstractSignalHandler *connect(
            const char *name,
            const Callback &callback)
    {
        if (auto handler = connectToSignal<Arguments...>(
                    m_connection,
                    m_path.data(),
                    m_interface,
                    name,
                    std::function<void(Arguments...)>(callback))) {
            handler->m_object = this;
            m_signals.push_back(handler);

            return handler;
        } else {
            return nullptr;
        }
    }

    template <typename Type> PropertySubscription *subscribe(
            const char *property, const std::function<void(Type)> &changed)
    {
        return subscribeToProperty(property, [changed](DBusMessageIter *iterator) {
            typename std::decay<Type>::type value;
            if (demarshall(iterator, value)) {
                changed(value);
            }
        });
    }

    void disconnectSignals();
    void unsubscribeProperties();
    void reset();

private:
    friend class PropertySubscription;
    friend class AbstractSignalHandler;

    PropertySubscription *subscribeToProperty(
            const char *name, const std::function<void(DBusMessageIter *)> &notify);
    inline void unsubscribe(PropertySubscription *subscription);

    static inline DBusHandlerResult property_change_callback(
            DBusConnection *, DBusMessage *message, void *data);

    const Connection m_connection;
    std::string m_rule;
    std::string m_path;
    std::vector<AbstractSignalHandler *> m_signals;
    std::vector<PropertySubscription *> m_subscriptions;
    const char * const m_service;
    const char * const m_interface;
};

class PendingGetProperty;

class PropertySubscription
{
public:
    void update();
    void unsubscribe();

private:
    friend class Object;
    friend class PendingGetProperty;

    PropertySubscription(
            Object *object, const char *name, const std::function<void(DBusMessageIter *)> &notify);
    ~PropertySubscription();

    std::string m_name;
    std::function<void(DBusMessageIter *)> m_notify;
    Object * const m_object;
    AbstractPendingCall *m_query;
};


}}

#endif
