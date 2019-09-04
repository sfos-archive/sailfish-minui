/*
 * Copyright (C) 2017-2019 Jolla Ltd.
 *
 * License: Proprietary
 */

#ifndef SAILFISH_MINUI_KEYPAD_H
#define SAILFISH_MINUI_KEYPAD_H

#include <sailfish-minui/icon.h>
#include <sailfish-minui/label.h>
#include <linux/input.h>

namespace Sailfish { namespace MinUi {

class Keypad;
class KeypadButton : public ResizeableItem
{
public:
    KeypadButton(int code, char character, Keypad *parent, const char *label = NULL);
    ~KeypadButton();

    virtual void setColor(Color color) = 0;

protected:
    void updateState(bool enabled);
    void activate() override;

private:
    Keypad *m_keypad;
    int m_code;
    char m_character;
    Label *m_label;
};

template <typename Decoration>
class KeypadButtonTemplate : public KeypadButton
{
public:
    KeypadButtonTemplate(const char *name, int code, char character, Keypad *parent, const char *label = NULL);
    ~KeypadButtonTemplate();

    Color color() const { return m_decoration.color(); }
    void setColor(Color color) override { m_decoration.setColor(color); }

protected:
    void layout();

private:
    Decoration m_decoration;
};

class Keypad : public ResizeableItem
{
public:
    explicit Keypad(Item *parent = nullptr);
    explicit Keypad(const char *acceptText, const char *cancelText, Item *parent = nullptr);
    ~Keypad();

    Palette palette() const { return m_palette; }
    void setPalette(const Palette &palette);

    bool isAcceptEnabled() const;
    void setAcceptEnabled(bool enabled);

    bool isAcceptVisible() const;
    void setAcceptVisible(bool visible);

    bool isCancelEnabled() const;
    void setCancelEnabled(bool enabled);

    bool isCancelVisible() const;
    void setCancelVisible(bool visible);

    void onKeyPress(const std::function<void(int code, char character)> &callback);

    /* Set new accept text, use null to use the default icon */
    void setAcceptText(const char *acceptText);
    /* Set new cancel text, use null to use the default icon */
    void setCancelText(const char *acceptText);

protected:
    void updateState(bool enabled) override;
    void layout() override;

private:
    friend class KeypadButton;

    inline void updateButtonState(KeypadButton *button, bool interactive) const;
    KeypadButton *cancelButton() const;
    KeypadButton *acceptButton() const;

    ResizeableItem m_buttonContainer { this };
    KeypadButtonTemplate<Icon> m_button1 { "sailfish-minui-bt-key1", 2 /*KEY_1*/, '1', this };
    //% "abc"
    KeypadButtonTemplate<Icon> m_button2 { "sailfish-minui-bt-key2", 3 /*KEY_2*/, '2', this, qtTrId("sailfish-minui-la-abc") };
    //% "def"
    KeypadButtonTemplate<Icon> m_button3 { "sailfish-minui-bt-key3", 4 /*KEY_3*/, '3', this, qtTrId("sailfish-minui-la-def") };
    //% "ghi"
    KeypadButtonTemplate<Icon> m_button4 { "sailfish-minui-bt-key4", 5 /*KEY_4*/, '4', this, qtTrId("sailfish-minui-la-ghi") };
    //% "jkl"
    KeypadButtonTemplate<Icon> m_button5 { "sailfish-minui-bt-key5", 6 /*KEY_5*/, '5', this, qtTrId("sailfish-minui-la-jkl") };
    //% "mno"
    KeypadButtonTemplate<Icon> m_button6 { "sailfish-minui-bt-key6", 7 /*KEY_6*/, '6', this, qtTrId("sailfish-minui-la-mno") };
    //% "pqrs"
    KeypadButtonTemplate<Icon> m_button7 { "sailfish-minui-bt-key7", 8 /*KEY_7*/, '7', this, qtTrId("sailfish-minui-la-pqrs") };
    //% "tuv"
    KeypadButtonTemplate<Icon> m_button8 { "sailfish-minui-bt-key8", 9 /*KEY_8*/, '8', this, qtTrId("sailfish-minui-la-tuv") };
    //% "wxyz"
    KeypadButtonTemplate<Icon> m_button9 { "sailfish-minui-bt-key9", 10 /*KEY_9*/, '9', this, qtTrId("sailfish-minui-la-wxyz") };
    KeypadButtonTemplate<Label> *m_cancelButton;
    KeypadButtonTemplate<Icon> *m_cancelIconButton;

    KeypadButtonTemplate<Icon> m_button0 { "sailfish-minui-bt-key0", 11 /*KEY_0*/, '0', this };
    KeypadButtonTemplate<Label> *m_acceptButton;
    KeypadButtonTemplate<Icon> *m_acceptIconButton;

    Palette m_palette;
    std::function<void(int code, char character)> m_keyPress;

    bool m_effectiveInteractive = true;
};

}}

#endif
