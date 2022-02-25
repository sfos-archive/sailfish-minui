/*
 * Copyright (c) 2022 Jolla Ltd.
 *
 * License: Proprietary
 */

#include "keyboard.h"

namespace Sailfish { namespace MinUi {

KeyboardButtonBase::KeyboardButtonBase(Keyboard *parent)
    : ResizeableItem(parent)
    , m_keyboard(parent)
{
    setCanActivate(true);
}

KeyboardButtonBase::~KeyboardButtonBase()
{
}

void KeyboardButtonBase::setState(KeyboardState)
{
    // nothing
}

void KeyboardButtonBase::activate()
{
    Window * const window = Item::window();

    if (window) {
        window->playHaptic(Window::KeyPressEffect);
    }

    m_keyboard->handleInput(m_code, m_character);
}


KeyboardButton::KeyboardButton(char char_lower, char char_upper, char char_symbol, Keyboard *parent)
    : KeyboardButtonBase(parent)
    , m_label(std::string(), this)
    , m_char_lower(char_lower)
    , m_char_upper(char_upper)
    , m_char_symbol(char_symbol)
{
    setState(KeyboardState::lowercase);
}

KeyboardButton::~KeyboardButton()
{
}

void KeyboardButton::setState(KeyboardState state)
{
    switch (state) {
    case KeyboardState::lowercase:
        m_character = m_char_lower;
        break;
    case KeyboardState::uppercase:
        m_character = m_char_upper;
        break;
    case KeyboardState::symbol:
        m_character = m_char_symbol;
        break;
    }
    m_label.setText(std::string(1, m_character));
}

void KeyboardButton::layout()
{
    m_label.centerIn(*this);
}

void KeyboardButton::updateState(bool enabled)
{
    (void) enabled;
    if (isPressed()) {
        m_label.setColor(m_keyboard->palette().pressed);
    } else {
        m_label.setColor(m_keyboard->palette().normal);
    }
}

SpaceButton::SpaceButton(Keyboard *parent)
    : KeyboardButtonBase(parent)
    , m_left("icon-m-spacebar-left", this)
    , m_middle("icon-m-spacebar-middle", this)
    , m_right("icon-m-spacebar-right", this)
{
    m_character = ' ';
}

SpaceButton::~SpaceButton()
{
}

void SpaceButton::layout()
{
    m_middle.centerIn(*this);

    m_left.align(Right, m_middle, Left);
    m_left.align(Bottom, m_middle, Bottom);

    m_right.align(Left, m_middle, Right);
    m_right.align(Bottom, m_middle, Bottom);
}

void SpaceButton::updateState(bool enabled)
{
    (void) enabled;
    Color color;
    if (isPressed()) {
        color = m_keyboard->palette().pressed;
    } else {
        color = m_keyboard->palette().normal;
    }

    m_left.setColor(color);
    m_right.setColor(color);
    m_middle.setColor(color);
}

KeyboardIconButton::KeyboardIconButton(Keyboard *parent, const char *icon, int keycode)
    : KeyboardButtonBase(parent)
    , m_icon(icon, this)
{
    m_code = keycode;
}

KeyboardIconButton::~KeyboardIconButton()
{
}

void KeyboardIconButton::layout()
{
    m_icon.centerIn(*this);
}

void KeyboardIconButton::updateState(bool enabled)
{
    setOpacity(enabled ? 1.0 : 0.6);
    if (isPressed()) {
        m_icon.setColor(m_keyboard->palette().pressed);
    } else {
        m_icon.setColor(m_keyboard->palette().normal);
    }
}

ShiftButton::ShiftButton(Keyboard *parent)
    : KeyboardButtonBase(parent)
    , m_shiftOff("icon-m-shift", this)
    , m_shiftOn("icon-m-shift-caps", this)
{
    m_code = KEY_LEFTSHIFT;
    setState(KeyboardState::lowercase);
}

ShiftButton::~ShiftButton()
{
}

void ShiftButton::setState(KeyboardState state)
{
    m_shiftOff.setVisible(state == KeyboardState::lowercase);
    m_shiftOn.setVisible(state == KeyboardState::uppercase);
    setEnabled(state != KeyboardState::symbol);
}

void ShiftButton::layout()
{
    m_shiftOff.centerIn(*this);
    m_shiftOn.centerIn(*this);
}

void ShiftButton::updateState(bool enabled)
{
    (void) enabled;
    Color color;
    if (isPressed()) {
        color = m_keyboard->palette().pressed;
    } else {
        color = m_keyboard->palette().normal;
    }
    m_shiftOn.setColor(color);
    m_shiftOff.setColor(color);
}

SymbolButton::SymbolButton(Keyboard *parent)
    : KeyboardButtonBase(parent)
    , m_label("", this)
{
    m_code = KEY_KEYBOARD; // picking something usable from evdev codes
    setState(KeyboardState::lowercase);
}

SymbolButton::~SymbolButton()
{
}

void SymbolButton::setState(KeyboardState state)
{
    if (state != KeyboardState::symbol) {
        m_label.setText("?123");
    } else {
        m_label.setText("ABC");
    }
}

void SymbolButton::layout()
{
    m_label.centerIn(*this);
}

void SymbolButton::updateState(bool enabled)
{
    (void) enabled;
    if (isPressed()) {
        m_label.setColor(m_keyboard->palette().pressed);
    } else {
        m_label.setColor(m_keyboard->palette().normal);
    }
}

namespace {
// Definition for a keyboard Qwerty layout.
const std::string row_1_lowercase {'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p'};
const std::string row_2_lowercase {'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l'};
const std::string row_3_lowercase {'z', 'x', 'c', 'v', 'b', 'n', 'm'};

const std::string row_1_uppercase {'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P'};
const std::string row_2_uppercase {'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L'};
const std::string row_3_uppercase {'Z', 'X', 'C', 'V', 'B', 'N', 'M'};

const std::string row_1_symbol {'1', '2', '3', '4', '5', '6', '7', '8', '9', '0'};
const std::string row_2_symbol {'*', '#', '+', '-', '=', '(', ')', '!', '?'};
const std::string row_3_symbol {'@', '&', '/', '\\', '\'', ';', ':'};
}

/*!
    \class Sailfish::MinUi::Keyboard
    \brief An alphanumeric input keyboard.
*/
Keyboard::Keyboard(Item *parent)
    : ResizeableItem(parent)
    , m_space(this)
    , m_symbolButton(this)
    , m_shift(this)
    , m_backspace(this, "icon-m-backspace", KEY_BACKSPACE)
    , m_enter(this, "icon-m-accept", KEY_ENTER)
{
    setItemFlags(NotifyOnInputFocusChanges);
    setHeight(4 * (theme.sizeCategory >= Theme::Large ? theme.itemSizeExtraLarge : theme.itemSizeLarge));
    createKeys();
}

/*!
    Destroys a keyboard.
*/
Keyboard::~Keyboard()
{
    for (const auto button : m_keyrow1) {
        delete button;
    }
    for (const auto button : m_keyrow2) {
        delete button;
    }
    for (const auto button : m_keyrow3) {
        delete button;
    }
    delete m_dotKey;
    delete m_commaKey;
}

bool Keyboard::enterEnabled() const
{
    return m_enter.isEnabled();
}

void Keyboard::setEnterEnabled(bool enabled)
{
    m_enter.setEnabled(enabled);
}

void Keyboard::handleInput(int code, char character)
{
    if (code == KEY_KEYBOARD) {
        setKeyState(m_state == KeyboardState::symbol ? KeyboardState::lowercase : KeyboardState::symbol);
        return;
    } else if (code == KEY_LEFTSHIFT) {
        setKeyState(m_state == KeyboardState::lowercase ? KeyboardState::uppercase : KeyboardState::lowercase);
        return;
    } else if (m_state == KeyboardState::uppercase) {
        setKeyState(KeyboardState::lowercase);
    }

    if (m_keyPress) {
        m_keyPress(code, character);
    } else {
        Window * const window = Item::window();
        if (window) {
            window->keyPress(code, character);
        }
    }
}

// only called for key taps that do output, i.e. not on shift or symbol view changes.
void Keyboard::onKeyPress(const std::function<void (int, char)> &callback)
{
    m_keyPress = callback;
}

/*
    Places a row of keys vertically aligned with item \a vertical and right above \horizontal item.
    The keys are resized to match \a width and \a height.
*/
static void placeRow(const std::vector<KeyboardButton*> &keyRow, ResizeableItem *horizontal,
                     ResizeableItem *vertical, const int width, const int height, int margin = 0)
{
    bool firstIter = true;
    ResizeableItem *prevItem = horizontal;
    for (const auto &key : keyRow) {
        key->resize(width, height);
        key->align(Left, *prevItem, (firstIter ? Left : Right), margin);
        key->align(Bottom , *vertical, Top);
        prevItem = key;
        margin = 0;
        firstIter = false;
    }
}

void Keyboard::layout()
{
    int keyHeight = height() / 4;
    int keyWidth = width() / m_keyrow1.size();
    int funcKeyWidth = (width() - m_keyrow3.size() * keyWidth) / 2;
    int largeFuncKeyWidth = funcKeyWidth * 1.25;
    int bottomKeyWidth = keyWidth * 1.25;
    int spaceWidth = width() - 2*largeFuncKeyWidth - 2* bottomKeyWidth;

    m_backspace.resize(funcKeyWidth, keyHeight);
    m_shift.resize(funcKeyWidth, keyHeight);

    m_enter.resize(largeFuncKeyWidth, keyHeight);
    m_symbolButton.resize(largeFuncKeyWidth, keyHeight);

    m_dotKey->resize(bottomKeyWidth, keyHeight);
    m_commaKey->resize(bottomKeyWidth, keyHeight);

    m_space.resize(spaceWidth, keyHeight);
    m_space.centerBetween(*this, Left, *this, Right);
    m_space.align(Bottom, *this, Bottom);
    m_enter.align(Bottom, *this, Bottom);
    m_enter.align(Right, *this, Right);

    m_backspace.align(Right, *this, Right);
    m_backspace.align(Bottom, m_space, Top);

    m_shift.align(Left, *this, Left);
    m_shift.align(Bottom, m_space, Top);

    m_symbolButton.align(Left, *this, Left);
    m_symbolButton.align(Bottom, *this, Bottom);

    m_commaKey->align(Left, m_symbolButton, Right);
    m_commaKey->align(Bottom, *this, Bottom);

    m_dotKey->align(Left, m_space, Right);
    m_dotKey->align(Bottom, *this, Bottom);

    placeRow(m_keyrow3, this, &m_space, keyWidth, keyHeight, funcKeyWidth);
    placeRow(m_keyrow2, this, m_keyrow3[0], keyWidth, keyHeight, keyWidth/2);
    placeRow(m_keyrow1, this, m_keyrow2[0], keyWidth, keyHeight);
}

void Keyboard::createKeys()
{
    auto createKeys = [this](const std::string &lowers, const std::string &uppers, const std::string &symbols) {
        size_t n = lowers.size();
        std::vector<KeyboardButton*> keys;
        for (size_t i = 0; i < n; ++i) {
            keys.push_back(new KeyboardButton(lowers[i], uppers[i], symbols[i], this));
        }
        return keys;
    };

    m_keyrow1 = createKeys(row_1_lowercase, row_1_uppercase, row_1_symbol);
    m_keyrow2 = createKeys(row_2_lowercase, row_2_uppercase, row_2_symbol);
    m_keyrow3 = createKeys(row_3_lowercase, row_3_uppercase, row_3_symbol);

    m_dotKey = new KeyboardButton('.', '.', '.', this);
    m_commaKey = new KeyboardButton(',', ',', ',', this);
    m_dotKey->setState(KeyboardState::lowercase);
    m_commaKey->setState(KeyboardState::lowercase);
}

void Keyboard::setKeyState(KeyboardState state)
{
    m_state = state;
    for (const auto &key : m_keyrow1) {
        key->setState(m_state);
    }
    for (const auto &key : m_keyrow2) {
        key->setState(m_state);
    }
    for (const auto &key : m_keyrow3) {
        key->setState(m_state);
    }

    m_symbolButton.setState(m_state);
    m_shift.setState(m_state);
}

}}
