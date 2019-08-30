/*
 * Copyright (C) 2017-2019 Jolla Ltd.
 *
 * License: Proprietary
 */

#ifndef SAILFISH_MINDBUS_POINTER_H
#define SAILFISH_MINDBUS_POINTER_H

namespace Sailfish { namespace MinDBus {

template <typename T> class Pointer
{
public:
    Pointer() : m_pointer(nullptr) {}
    Pointer(T *pointer) : m_pointer(pointer) {}
    Pointer(const Pointer<T> &pointer)
        : m_pointer(pointer.m_pointer)
    {
        reference(m_pointer);
    }
    ~Pointer()
    {
        release(m_pointer);
    }

    Pointer &operator =(const Pointer<T> &pointer)
    {
        if (pointer.m_pointer) {
            reference(pointer.m_pointer);
        }
        T * const old = m_pointer;
        m_pointer = pointer.m_pointer;
        if (old) {
            release(old);
        }
        return *this;
    }

    operator T *() const { return m_pointer; }
    operator bool() const { return m_pointer; }

    T *operator ->() const { return m_pointer; }
    bool operator !() const { return !m_pointer; }

    T *take()
    {
        T *pointer = m_pointer;
        m_pointer = nullptr;
        return pointer;
    }

    void reset(T *pointer)
    {
        T *old = m_pointer;
        m_pointer = pointer;
        if (old) {
            release(old);
        }
    }

    T *data() const { return m_pointer; }

    static inline void reference(T *pointer);
    static inline void release(T *pointer);

private:
    T *m_pointer;
};

}}

#endif
