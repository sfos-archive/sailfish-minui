/*
 * Copyright (c) 2017-2019 Jolla Ltd.
 *
 * License: Proprietary
 */

#include "pendingcall.h"

namespace Sailfish { namespace MinDBus {

AbstractPendingCall::AbstractPendingCall(DBusPendingCall *call)
    : m_call(call)
{
    dbus_pending_call_set_notify(call, notify_callback, this, free_callback);
}

AbstractPendingCall::~AbstractPendingCall()
{
}

void AbstractPendingCall::cancel()
{
    dbus_pending_call_cancel(m_call);
    dbus_pending_call_unref(m_call);

    release();
}

void AbstractPendingCall::release()
{
    if (--m_ref == 0) {
        delete this;
    }
}

void AbstractPendingCall::notify_callback(DBusPendingCall *call, void *data)
{
    auto instance = static_cast<AbstractPendingCall *>(data);

    {
        const Message message = dbus_pending_call_steal_reply(call);

        instance->notify(message);
    }

    dbus_pending_call_unref(call);

    instance->release();
}

void AbstractPendingCall::free_callback(void *data)
{
    static_cast<AbstractPendingCall *>(data)->release();
}

}}
