/****************************************************************************************
** Copyright (c) 2019 - 2023 Jolla Ltd.
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

#include "display.h"
#include "logging.h"
#include <minui/minui.h>

namespace Sailfish { namespace MinUi {

Display *Display::s_instance = nullptr;

bool Display::isPoweredOn(State state)
{
    bool poweredOn = false;
    switch (state) {
    case On:
    case Doze:
    case DozeSuspend:
        poweredOn = true;
        break;
    default:
        break;
    }
    return poweredOn;
}

bool Display::isDrawable(State state)
{
    bool drawable = false;
    switch (state) {
    case On:
    case Doze:
        drawable = true;
        break;
    default:
        break;
    }
    return drawable;
}

Display *Display::instance()
{
    if (!s_instance) {
      log_debug("Display instance created");
      s_instance = new Display();
    }
    return s_instance;
}

Display::Display()
    : m_state(Unknown)
{
}

Display::~Display()
{
}

Display::State Display::state() const
{
    return m_state;
}

bool Display::isDrawable() const
{
    return isDrawable(state());
}

bool Display::isPoweredOn() const
{
    return isPoweredOn(state());
}

void Display::setState(State state)
{
    if (m_state != state) {
        log_debug("Display state:" << state);
        bool wasPoweredOn = isPoweredOn();
        m_state = state;
        bool poweredOn = isPoweredOn();
        if (wasPoweredOn != poweredOn) {
            log_debug("Display poweredOn:" << poweredOn);
            gr_fb_blank(!poweredOn);
        }
    }
}

void Display::blank()
{
    setState(Off);
}

void Display::unblank()
{
    setState(On);
}

}}
