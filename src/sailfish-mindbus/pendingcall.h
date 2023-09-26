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

#ifndef SAILFISH_MINDBUS_PENDINGCALL_H
#define SAILFISH_MINDBUS_PENDINGCALL_H


#include <sailfish-mindbus/message.h>
#include <string.h>

namespace Sailfish { namespace MinDBus {

class AbstractPendingCall
{
public:
    explicit AbstractPendingCall(DBusPendingCall *call);

    void cancel();

protected:
    virtual void notify(DBusMessage *message) = 0;

    virtual ~AbstractPendingCall();

private:
    inline void release();

    static inline void notify_callback(DBusPendingCall *call, void *data);
    static inline void free_callback(void *data);

    DBusPendingCall * const m_call;
    int m_ref = 2;
};

template <typename... Arguments> class PendingCall : public AbstractPendingCall
{
public:
    PendingCall(DBusPendingCall *call) : AbstractPendingCall(call) {}

    void onFinished(const std::function<void(Arguments...)> &handler)
    {
        m_finished = handler;
    }

    void onError(const std::function<void(const char *name, const char *message)> &handler)
    {
        m_error = handler;
    }

protected:
    void notify(DBusMessage *message) override
    {
        switch (dbus_message_get_type(message)) {
        case DBUS_MESSAGE_TYPE_METHOD_RETURN:
            if (m_finished) {
                invoke(message, m_finished);
            }
            break;
        case DBUS_MESSAGE_TYPE_ERROR: {
            const char *description;
            if (m_error && dbus_message_get_args(message, nullptr, DBUS_TYPE_STRING, &description, DBUS_TYPE_INVALID)) {
                m_error(dbus_message_get_error_name(message), description);
            }
            break;
        }
        default:
            break;
        }
    }

private:
    std::function<void(Arguments...)> m_finished;
    std::function<void(const char *name, const char *message)> m_error;
};

}}

#endif
