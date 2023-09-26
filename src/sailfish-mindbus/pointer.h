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
        if (m_pointer)
            reference(m_pointer);
    }
    ~Pointer()
    {
        if (m_pointer)
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
