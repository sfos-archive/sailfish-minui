/****************************************************************************
**
** Copyright (C) 2017 Jolla Ltd.
** Contact: Andrew den Exter <andrew.den.exter@jolla.com>
**
****************************************************************************/

#ifndef SAILFISH_MINUI_KEYPAD_H
#define SAILFISH_MINUI_KEYPAD_H

#include <sailfish-minui/icon.h>
#include <sailfish-minui/label.h>

namespace Sailfish { namespace MinUi {

class Keypad;
class KeypadButton : public ResizeableItem
{
public:
    KeypadButton(int code, char character, Keypad *parent);
    ~KeypadButton();

    virtual void setColor(Color color) = 0;

protected:
    void updateState(bool enabled);
    void activate() override;

private:
    Keypad *m_keypad;
    int m_code;
    char m_character;
};

template <typename Decoration>
class KeypadButtonTemplate : public KeypadButton
{
public:
    KeypadButtonTemplate(const char *name, int code, char character, Keypad *parent);
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

    bool isAcceptEnabled() const { return m_acceptEnabled; }
    void setAcceptEnabled(bool enabled);

    bool isAcceptVisible() const { return m_acceptButton.isVisible(); }
    void setAcceptVisible(bool visible) { m_acceptButton.setVisible(visible); }

    bool isCancelEnabled() const { return m_cancelEnabled; }
    void setCancelEnabled(bool enabled);

    bool isCancelVisible() const { return m_cancelButton.isVisible(); }
    void setCancelVisible(bool visible) { m_cancelButton.setVisible(visible); }

    void onKeyPress(const std::function<void(int code, char character)> &callback);

protected:
    void updateState(bool enabled) override;
    void layout() override;

private:
    friend class KeypadButton;

    inline void updateButtonState(KeypadButton *button, bool interactive) const;

    ResizeableItem m_buttonContainer { this };
    KeypadButtonTemplate<Icon> m_button1 { "icon-l-key1", 2 /*KEY_1*/, '1', this };
    KeypadButtonTemplate<Icon> m_button2 { "icon-l-key2", 3 /*KEY_2*/, '2', this };
    KeypadButtonTemplate<Icon> m_button3 { "icon-l-key3", 4 /*KEY_3*/, '3', this };
    KeypadButtonTemplate<Icon> m_button4 { "icon-l-key4", 5 /*KEY_4*/, '4', this };
    KeypadButtonTemplate<Icon> m_button5 { "icon-l-key5", 6 /*KEY_5*/, '5', this };
    KeypadButtonTemplate<Icon> m_button6 { "icon-l-key6", 7 /*KEY_6*/, '6', this };
    KeypadButtonTemplate<Icon> m_button7 { "icon-l-key7", 8 /*KEY_7*/, '7', this };
    KeypadButtonTemplate<Icon> m_button8 { "icon-l-key8", 9 /*KEY_8*/, '8', this };
    KeypadButtonTemplate<Icon> m_button9 { "icon-l-key9", 10 /*KEY_9*/, '9', this };
    KeypadButtonTemplate<Label> m_cancelButton;
    KeypadButtonTemplate<Icon> m_button0 { "icon-l-key0", 11 /*KEY_0*/, '0', this };
    KeypadButtonTemplate<Label> m_acceptButton;

    Palette m_palette;
    std::function<void(int code, char character)> m_keyPress;

    bool m_acceptEnabled = true;
    bool m_cancelEnabled = true;
    bool m_effectiveInteractive = true;
};

}}

#endif
