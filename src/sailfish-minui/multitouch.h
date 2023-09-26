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

#ifndef SAILFISH_MINUI_MULTITOUCH_H
#define SAILFISH_MINUI_MULTITOUCH_H

extern "C" struct input_event;

namespace Sailfish { namespace MinUi {

class MultiTouch
{
public:
    typedef void (*EventCallback)(int x, int y, void *callbackData);

    MultiTouch(EventCallback fingerPressed,
               EventCallback fingerMoved,
               EventCallback fingerLifted,
               void *callbackData);
    void inputEvent(int fd, const input_event &event);
    int multitouchFd() const;

private:
    /** Known multitouch input device types */
    enum Type {
        None,
        ProtocolA,
        ProtocolB,
    };

    enum {
        /** Maximum number of touchpoints to track */
        MaxFingers = 16,

        /** Minimum finger movement distance to report
         *
         * [in touch panel coordinates]
         */
        DragThreshold = 3,
    };

    /** State data for individual touch points */
    struct TouchPoint
    {
        int id;
        int x;
        int y;

        TouchPoint()
            : id(-1), x(-1), y(-1) { }

        bool filled() const {
            return x != -1 && y != -1;
        }

        void clear() {
            id = -1, x = -1, y = -1;
        }
    };

    /** Detected multitouch input device type */
    Type m_type = Type::None;

    /** File descriptor of the device for which the type applies */
    int  m_fd = -1;

    /** The tracked finger position */
    TouchPoint m_active;

    /** Previously reported position for filtering purposes */
    TouchPoint m_prev;

    /** Protocol A: Touchpoint state data
     *
     * accumulate data via X, Y and ID events
     * push to m_point[m_points++] on SYN_MT_REPORT
     * check touchstate on SYN_REPORT
     */
    TouchPoint m_state;

    /** Protocol A: number of touchpoints reported */
    int        m_points = 0;

    /** Protocol B touchpoint slot
     *
     * choose slot on SLOT_ID event
     * modify m_point[m_slot] on X, Y and ID events
     * check touchstate on SYN_REPORT
     */
    int        m_slot = 0;

    /** Array for holding data on all reported fingers
     *
     * For protocol A: the entries 0 ... m_points are valid.
     *
     * For protocol B: entries with valid id are valid.
     *
     * NB: Finger positions exceeding MaxFingers are silently
     *     ignored.
     */
    TouchPoint m_point[MaxFingers];

    /** Flag for: Some sort of touch on screen
     *
     * NB: Only the first finger on screen is reported (and
     *     and all fingers need to be lifted before another
     *     touchpoint is reported).
     */
    bool       m_tracking = false;

    /** Flag for: The 1st detected finger still on screen */
    bool       m_touching = false;

    /** Tracking id of the 1st detected finger */
    int        m_touch_id = -1;

    /** Callback for finger pressed notifications */
    EventCallback m_fingerPressed;

    /** Callback for finger moved notifications */
    EventCallback m_fingerMoved;

    /** Callback for finger lifted notifications */
    EventCallback m_fingerLifted;

    /** Context to pass to fingerPressed/Moved/Lifted notifications */
    void *m_callbackData;

    void updateTouching(bool touching);
    void evaluateTouchingA();
    void evaluateTouchingB();
    void handleProtocolA(const input_event &event);
    void handleProtocolB(const input_event &event);
    bool isMultiTouchDevice(int fd);
};

}}
#endif /* SAILFISH_MINUI_MULTITOUCH_H */
