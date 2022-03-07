/*
 * Copyright (c) 2017-2019 Jolla Ltd.
 *
 * License: Proprietary
 */

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
