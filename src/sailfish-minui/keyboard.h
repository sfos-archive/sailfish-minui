/*
 * Copyright (c) 2022 Jolla Ltd.
 *
 * License: Proprietary
 */

#ifndef SAILFISH_MINUI_KEYBOARD_H
#define SAILFISH_MINUI_KEYBOARD_H

#include <sailfish-minui/icon.h>
#include <sailfish-minui/label.h>
#include <linux/input.h>

#include <vector>

namespace Sailfish { namespace MinUi {

class Keyboard;
enum class KeyboardState {lowercase, uppercase, symbol};

class KeyboardButtonBase : public ResizeableItem
{
public:
    KeyboardButtonBase(Keyboard *parent);
    ~KeyboardButtonBase();

    virtual void setState(KeyboardState state);

protected:
    void activate() override;

    Keyboard *m_keyboard;
    int m_code = 0;
    char m_character = '\0';
};

class KeyboardButton : public KeyboardButtonBase
{
public:
    KeyboardButton(char char_lower, char char_upper, char char_symbol, Keyboard *parent);
    ~KeyboardButton();

    void setState(KeyboardState state);

protected:
    void layout();
    void updateState(bool enabled);

private:
    LiteralLabel m_label;

    char m_char_lower;
    char m_char_upper;
    char m_char_symbol;
};

class SpaceButton : public KeyboardButtonBase
{
public:
    SpaceButton(Keyboard *parent);
    ~SpaceButton();

protected:
    void layout();
    void updateState(bool enabled);

private:
    Icon m_left;
    Icon m_middle;
    Icon m_right;
};

class KeyboardIconButton : public KeyboardButtonBase
{
public:
    KeyboardIconButton(Keyboard *parent, const char *icon, int keycode);
    ~KeyboardIconButton();

protected:
    void layout();
    void updateState(bool enabled);

private:
    Icon m_icon;
};

class ShiftButton : public KeyboardButtonBase
{
public:
    ShiftButton(Keyboard *parent);
    ~ShiftButton();

    void setState(KeyboardState state);

protected:
    void layout();
    void updateState(bool enabled);

private:
    Icon m_shiftOff;
    Icon m_shiftOn;
};

class SymbolButton : public KeyboardButtonBase
{
public:
    SymbolButton(Keyboard *parent);
    ~SymbolButton();

    void setState(KeyboardState state);

protected:
    void layout();
    void updateState(bool enabled);

private:
    LiteralLabel m_label;
};

class Keyboard : public ResizeableItem
{
public:
    explicit Keyboard(Item *parent = nullptr);
    ~Keyboard();

    bool enterEnabled() const;
    void setEnterEnabled(bool enabled);

    void onKeyPress(const std::function<void(int code, char character)> &callback);

    Palette palette() const { return m_palette; }
    void handleInput(int code, char character);

protected:
    void layout() override;

private:
    void createKeys();
    void setKeyState(KeyboardState state);

    std::function<void(int code, char character)> m_keyPress;
    KeyboardState m_state {KeyboardState::lowercase};
    Palette m_palette;

    SpaceButton m_space;
    SymbolButton m_symbolButton;
    ShiftButton m_shift;
    KeyboardIconButton m_backspace;
    KeyboardIconButton m_enter;
    KeyboardButton* m_dotKey;
    KeyboardButton* m_commaKey;
    std::vector<KeyboardButton*> m_keyrow1;
    std::vector<KeyboardButton*> m_keyrow2;
    std::vector<KeyboardButton*> m_keyrow3;
};

}}

#endif
