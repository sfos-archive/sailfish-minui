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

#ifndef SAILFISH_MINDBUS_SIGNAL_H
#define SAILFISH_MINDBUS_SIGNAL_H

#include <sailfish-mindbus/message.h>

#include <string>

namespace Sailfish { namespace MinDBus {

class Object;
class AbstractSignalHandler
{
public:
    explicit AbstractSignalHandler(
            DBusConnection *connection, const char *path, const char *interface, const char *name);

    void disconnect();

protected:
    virtual void notify(DBusMessage *message) = 0;

    virtual ~AbstractSignalHandler();

private:
    friend class Object;

    inline void release();

    static inline DBusHandlerResult notify_callback(
            DBusConnection *connection, DBusMessage *message, void *data);
    static inline void free_callback(void *data);

    std::string m_rule;
    std::string m_path;
    DBusConnection * const m_connection;
    const char * const m_interface;
    const char * const m_name;
    Object *m_object = nullptr;
    int m_ref = 2;
};

template <typename... Arguments> class SignalHandler : public AbstractSignalHandler
{
public:
    SignalHandler(
            DBusConnection *connection,
            const char *path,
            const char *interface,
            const char *name,
            const std::function<void(Arguments...)> &handler)
        : AbstractSignalHandler(connection, path, interface, name)
        , m_handler(handler)

    {
    }

protected:
    void notify(DBusMessage *message) override
    {
        invoke(message, m_handler);
    }

private:
    std::function<void(Arguments...)> m_handler;
};

template <typename... Arguments> AbstractSignalHandler *connectToSignal(
        DBusConnection *connection,
        const char *path,
        const char *interface,
        const char *name,
        const std::function<void(Arguments...)> &handler)
{
    return new SignalHandler<Arguments...>(connection, path, interface, name, handler);
}

}}

#endif
