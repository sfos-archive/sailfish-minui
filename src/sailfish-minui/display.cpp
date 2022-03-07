/*
 * Copyright (c) 2019 Jolla Ltd.
 *
 * License: Proprietary
 */

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
