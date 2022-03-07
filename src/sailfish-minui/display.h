/*
 * Copyright (c) 2019 Jolla Ltd.
 *
 * License: Proprietary
 */

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
