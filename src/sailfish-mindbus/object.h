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
