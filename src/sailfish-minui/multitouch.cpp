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

#include "multitouch.h"
#include "logging.h"

#include <linux/input.h>

#include <stdio.h>
#include <limits.h>
#include <string.h>
#include <errno.h>

#include <string>

#define BMAP_SIZE(bc) (((bc)+LONG_BIT-1)/LONG_BIT)

namespace Sailfish { namespace MinUi {

static const char *errnoString() {
    char *errorName = strerror(errno);
    return errorName ? errorName : "unknown";
}

static bool bitIsSet(unsigned long *bmap, unsigned bi)
{
    unsigned i = bi / LONG_BIT;
    unsigned long m = 1ul << (bi % LONG_BIT);
    return (bmap[i] & m) != 0;
}

MultiTouch::MultiTouch(EventCallback fingerPressed,
                       EventCallback fingerMoved,
                       EventCallback fingerLifted,
                       void *callbackData)
    : m_fingerPressed(fingerPressed)
    , m_fingerMoved(fingerMoved)
    , m_fingerLifted(fingerLifted)
    , m_callbackData(callbackData)

{
}

int MultiTouch::multitouchFd() const
{
    return m_type != Type::None ? m_fd : -1;
}

void MultiTouch::updateTouching(bool touching)
{
    if (m_touching != touching) {
        if ((m_touching = touching)) {
            m_prev = m_active;
            m_fingerPressed(m_active.x, m_active.y, m_callbackData);
        } else {
            m_fingerLifted(m_active.x, m_active.y, m_callbackData);

            /* Especially protocol A is likely to have recycled
             * id numbers to be seen -> forget tracking id once
             * we have seen the corresponding finger lifted. */
            m_touch_id = -1;
        }
    } else if (m_touching) {
        int u = m_prev.x - m_active.x;
        int v = m_prev.y - m_active.y;
        if (u*u + v*v >= DragThreshold*DragThreshold) {
            m_prev = m_active;
            m_fingerMoved(m_active.x, m_active.y, m_callbackData);
        }
    }
}

void MultiTouch::evaluateTouchingA()
{
    /* Assume: Tracked finger was lifted */
    bool touching = false;

    if (m_tracking) {
        /* Tracking: Ends when all fingers are lifted */
        m_tracking = (m_points > 0);

        /* Touching: While tracked touchpoint remains */
        if (m_touch_id != -1) {
            for (int i = 0; i < m_points; ++i) {
                if (m_point[i].id != m_touch_id)
                    continue;
                touching = true;
                m_active = m_point[i];
                break;
            }
        }
    } else {
        /* Tracking: Starts when 1st finger detected */
        for (int i = 0; i < m_points; ++i) {
            if (m_point[i].id == -1)
                continue;
            touching = true;
            m_tracking = true;
            m_touch_id = m_point[i].id;
            m_active = m_point[i];
            break;
        }
    }

    /* Reset protocol A touchpoint count */
    m_points = 0;

    /* Update status */
    updateTouching(touching);
}

void MultiTouch::evaluateTouchingB()
{
    /* Assume: Tracked finger was lifted */
    bool touching = false;

    if (m_tracking) {
        /* Assume: All fingers were lifted & tracking ends */
        m_tracking = false;
        for (int i = 0; i < MaxFingers; ++i) {

            if (m_point[i].id == -1)
                continue;

            /* Have a finger on screeen, tracking continues */
            m_tracking = true;

            if (m_point[i].id != m_touch_id)
                continue;

            /* The tracked finger is still on screen */
            touching = true;
            m_active = m_point[i];
            break;
        }
    } else {
        /* Tracking: Starts when 1st finger detected */
        for (int i = 0; i < MaxFingers; ++i) {
            if (m_point[i].id == -1)
                continue;
            touching = true;
            m_tracking = true;
            m_touch_id = m_point[i].id;
            m_active = m_point[i];
            break;
        }
    }

    /* Update status */
    updateTouching(touching);
}

void MultiTouch::handleProtocolA(const input_event &event)
{
    switch (event.type) {
    case EV_ABS:
        switch (event.code) {
        case ABS_MT_TRACKING_ID:
            m_state.id = event.value;
            break;
        case ABS_MT_POSITION_X:
            m_state.x = event.value;
            break;
        case ABS_MT_POSITION_Y:
            m_state.y = event.value;
            break;
        default:
            break;
        }
        break;

    case EV_SYN:
        switch (event.code) {
        case SYN_MT_REPORT:
            if (m_state.filled() && m_points < MaxFingers) {
                /* NB Tracking ID is optional for ProtocolA.
                 *
                 * When not reported, one might attempt to match
                 * previously reported touchpoints to current
                 * ones. But for single finger happy path it
                 * suffices to use dummy zero id ...
                 */
                if (m_state.id == -1)
                    m_state.id = 0;
                m_point[m_points++] = m_state;
            }
            m_state.clear();
            break;
        case SYN_REPORT:
            evaluateTouchingA();
            break;
        default:
            break;
        }
        break;

    default:
        break;
    }
}

void MultiTouch::handleProtocolB(const input_event &event)
{
    switch (event.type) {
    case EV_ABS:
        switch (event.code) {
        case ABS_MT_SLOT:
            m_slot = event.value;
            break;
        case ABS_MT_TRACKING_ID:
            if ((unsigned)m_slot < (unsigned)MaxFingers)
                m_point[m_slot].id = event.value;
            break;
        case ABS_MT_POSITION_X:
            if ((unsigned)m_slot < (unsigned)MaxFingers)
                m_point[m_slot].x = event.value;
            break;
        case ABS_MT_POSITION_Y:
            if ((unsigned)m_slot < (unsigned)MaxFingers)
                m_point[m_slot].y = event.value;
            break;
        default:
            break;
        }
        break;

    case EV_SYN:
        switch (event.code) {
        case SYN_REPORT:
            evaluateTouchingB();
            break;
        default:
            break;
        }
        break;

    default:
        break;
    }
}

bool MultiTouch::isMultiTouchDevice(int fd)
{
    unsigned long types[BMAP_SIZE(EV_CNT)];
    unsigned long codes[BMAP_SIZE(KEY_CNT)];

    if (m_fd == fd) {
        /* Already probed */
    }
    else if (m_type != Type::None) {
        /* Another fd already chosen */
    } else {
        /* Mark down: This fd got probed */
        m_fd = fd;

        if (ioctl(fd, EVIOCGBIT(0, EV_CNT), memset(types, 0, sizeof types)) == -1) {
            /* No event type info -> is it even an input device? */
            log_warning("failed to probe event types: " << errnoString());
        } else if (!bitIsSet(types, EV_ABS)) {
            /* No EV_ABS events -> not a multitouch input device */
        } else if (ioctl(fd, EVIOCGBIT(EV_ABS, ABS_CNT), memset(codes, 0, sizeof codes)) == -1) {
            /* No EV_ABS info -> further probing is useless */
            log_warning("failed to probe EV_ABS event codes: " << errnoString());
        } else if (!bitIsSet(codes, ABS_MT_POSITION_X)) {
            /* No ABS_MT_POSITION_X events -> not a mt input device */
        } else if (!bitIsSet(codes, ABS_MT_SLOT)) {
            /* Uses multitouch Protocol A */
            m_type = Type::ProtocolA;
            log_debug("Found multitouch protocol A input device");
        } else {
            /* Uses multitouch Protocol B */
            m_type = Type::ProtocolB;
            log_debug("Found multitouch protocol B input device");

            /* Seed delta processing by synthetizing events matching
             * the current input device state.
             *
             * Note: SYN_REPORT is omitted on purpose so that this
             *       does not turn into a false positive press.
             */
            static const int lut[] = {
                ABS_MT_SLOT,
                ABS_MT_TRACKING_ID,
                ABS_MT_POSITION_X,
                ABS_MT_POSITION_Y,
                -1
            };
            struct input_absinfo info;
            struct input_event event;
            memset(&info, 0, sizeof info);
            memset(&event, 0, sizeof event);
            for (int i = 0; lut[i] != -1; ++i) {
                if (ioctl(fd, EVIOCGABS(lut[i]), &info) == 0) {
                    event.type  = EV_ABS;
                    event.code  = lut[i];
                    event.value = info.value;
                    handleProtocolB(event);
                }
            }
        }
    }

    return m_fd == fd && m_type != Type::None;
}

void MultiTouch::inputEvent(int fd, const input_event &event)
{
    if (isMultiTouchDevice(fd)) {
        switch (m_type) {
        case Type::ProtocolA:
            handleProtocolA(event);
            break;
        case Type::ProtocolB:
            handleProtocolB(event);
            break;
        default:
            break;
        }
    }
}

}}
