/*
 * Copyright (c) 2017-2019 Jolla Ltd.
 *
 * License: Proprietary
 */

#ifndef SAILFISH_MINDBUS_MESSAGE_H
#define SAILFISH_MINDBUS_MESSAGE_H

#include <sailfish-mindbus/pointer.h>

#include <dbus/dbus.h>

#include <functional>
#include <string>
#include <utility>

namespace Sailfish { namespace MinDBus {

typedef Pointer<DBusMessage> Message;

template <> inline void Pointer<DBusMessage>::reference(DBusMessage *pointer)
{
    dbus_message_ref(pointer);
}

template <> inline void Pointer<DBusMessage>::release(DBusMessage *pointer)
{
    dbus_message_unref(pointer);
}

class ObjectPath
{
public:
    ObjectPath() : path(nullptr) {}
    ObjectPath(const char *path) : path(path) {}

    operator const char *() const { return path; }

    const char *path;
};

template <typename T> struct Traits;
template <typename T> struct Traits<const T &> { typedef typename Traits<T>::Type Type; enum : int { TypeId = Traits<T>::TypeId }; };
template <> struct Traits<bool> { typedef dbus_bool_t Type; enum : int { TypeId = DBUS_TYPE_BOOLEAN }; };
template <> struct Traits<int32_t> { typedef int32_t Type; enum : int { TypeId = DBUS_TYPE_INT32 }; };
template <> struct Traits<uint32_t> { typedef uint32_t Type; enum : int { TypeId = DBUS_TYPE_UINT32 }; };
template <> struct Traits<dbus_int64_t > { typedef dbus_int64_t Type; enum : int { TypeId = DBUS_TYPE_INT64 }; };
template <> struct Traits<dbus_uint64_t > { typedef dbus_uint64_t  Type; enum : int { TypeId = DBUS_TYPE_UINT64 }; };
template <> struct Traits<const char *> { typedef const char * Type; enum : int { TypeId = DBUS_TYPE_STRING }; };
template <> struct Traits<ObjectPath> { typedef const char * Type; enum : int { TypeId = DBUS_TYPE_OBJECT_PATH }; };

template <typename T> inline void marshall(DBusMessageIter *iterator, T value) {
    typename Traits<T>::Type data = value;
    dbus_message_iter_append_basic(iterator, Traits<T>::TypeId, &data);
}
template <typename T> inline bool demarshall(DBusMessageIter *iterator, T &value) {
    if (dbus_message_iter_get_arg_type(iterator) == Traits<T>::TypeId) {
        typename Traits<T>::Type data;
        dbus_message_iter_get_basic(iterator, &data);
        value = data;
        return true;
    } else {
        return false;
    }
}

template <> inline void marshall<const ObjectPath &>(DBusMessageIter *iterator, const ObjectPath &value) {
    dbus_message_iter_append_basic(iterator, DBUS_TYPE_OBJECT_PATH, &value.path);
}

template <> inline bool demarshall<ObjectPath>(DBusMessageIter *iterator, ObjectPath &value) {
    if (dbus_message_iter_get_arg_type(iterator) == DBUS_TYPE_OBJECT_PATH) {
        dbus_message_iter_get_basic(iterator, &value.path);
        return true;
    } else {
        return false;
    }
}

inline bool demarshallArguments(DBusMessageIter *) { return true; }

template <typename Argument, typename... Arguments>
bool demarshallArguments(DBusMessageIter *iterator, Argument &argument, Arguments&... arguments)
{
    if (demarshall(iterator, argument)) {
        dbus_message_iter_next(iterator);
        return demarshallArguments(iterator, arguments...);
    } else {
        return false;
    }
}

inline void invoke(DBusMessage *, const std::function<void()> &handler)
{
    handler();
}

template <typename Argument0>
inline void invoke(DBusMessage *message, const std::function<void(Argument0)> &handler)
{
    typename std::decay<Argument0>::type argument0;

    DBusMessageIter iterator;
    if (dbus_message_iter_init(message, &iterator) && demarshallArguments(&iterator, argument0)) {
        handler(argument0);
    }
}

template <typename Argument0, typename Argument1>
inline void invoke(DBusMessage *message, const std::function<void(Argument0, Argument1)> &handler)
{
    typename std::decay<Argument0>::type argument0;
    typename std::decay<Argument1>::type argument1;

    DBusMessageIter iterator;
    if (dbus_message_iter_init(message, &iterator)
            && demarshallArguments(&iterator, argument0, argument1)) {
        handler(argument0, argument1);
    }
}

template <typename Argument0, typename Argument1, typename Argument2>
inline void invoke(
        DBusMessage *message,
        const std::function<void(Argument0, Argument1, Argument2)> &handler)
{
    typename std::decay<Argument0>::type argument0;
    typename std::decay<Argument1>::type argument1;
    typename std::decay<Argument2>::type argument2;

    DBusMessageIter iterator;
    if (dbus_message_iter_init(message, &iterator)
            && demarshallArguments(&iterator, argument0, argument1, argument2)) {
        handler(argument0, argument1, argument2);
    }
}

inline void marshallArguments(DBusMessageIter *) {}

template <typename Argument, typename... Arguments>
inline void marshallArguments(DBusMessageIter *iterator, Argument argument, Arguments... arguments)
{
    marshall(iterator, argument);
    marshallArguments(iterator, arguments...);
}

template <typename... Arguments> DBusMessage *createMethodCall(
        const char *service,
        const char *path,
        const char *interface,
        const char *name,
        Arguments... arguments)
{
    DBusMessage *message = dbus_message_new_method_call(service, path, interface, name);

    DBusMessageIter iterator;
    dbus_message_iter_init_append(message, &iterator);
    marshallArguments(&iterator, arguments...);
    return message;
}

}}

#endif
