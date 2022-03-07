/*
 * Copyright (c) 2017-2019 Jolla Ltd.
 *
 * License: Proprietary
 */

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
