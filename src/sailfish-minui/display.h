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

#ifndef SAILFISH_MINUI_DISPLAY_H_
#define SAILFISH_MINUI_DISPLAY_H_

namespace Sailfish { namespace MinUi {
class Display
{
public:
    /* For forward compatibility: Keep State enum feature wise
     * compatible with setPowerMode() from android hw composer API
     * - which uses HWC_POWER_MODE_OFF|NORMAL|DOZE|DOZE_SUSPEND.
     */
    enum State {
        /* Until 1st explicit blank/unblank - caveat coder */
        Unknown = -1,
        /* Display is powered off */
        Off,
        /* Display is powered on, in normal mode */
        On,
        /* Display is powered on, in updatable low power mode */
        Doze,
        /* Display is powered on, in static low power mode */
        DozeSuspend,
    };
    static bool isPoweredOn(State state);
    static bool isDrawable(State state);
    static Display *instance();
    State state() const;
    bool isPoweredOn() const;
    bool isDrawable() const;
    void setState(State state);
    void blank();
    void unblank();
private:
    explicit Display();
    ~Display();
    State m_state;
    static Display *s_instance;
};
}}

#endif /* SAILFISH_MINUI_DISPLAY_H_ */
