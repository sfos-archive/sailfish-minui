/*
 * Copyright (c) 2017-2019 Jolla Ltd.
 *
 * License: Proprietary
 */

#include "keypad.h"

namespace Sailfish { namespace MinUi {

/*!
    \class Sailfish::MinUi::KeypadButton
    \brief A button in a keypad.
    \internal
*/

/*!
    Constructs a button member of a \a keypad for a given \a key \a code and ASCII \a character
    combination.
*/
KeypadButton::KeypadButton(int code, char character, Keypad *keypad, const char *label)
    : ResizeableItem(&keypad->m_buttonContainer)
    , m_keypad(keypad)
    , m_code(code)
    , m_character(character)
    , m_label(nullptr)
{
    setAcceptsKeyFocus(true);
    setKeyFocusOnPress(true);
    setCanActivate(true);

    resize(theme.itemSizeHuge, theme.sizeCategory >= Theme::Large ? theme.itemSizeExtraLarge : theme.itemSizeLarge);

    if (label) {
        m_label = new Label(label, this);
        m_label->centerBetween(*this, Left, *this, Right);
        m_label->align(Bottom, *this, Bottom);
        m_label->setOpacity(0.6);
    }
}

/*!
    Destroys a key pad button.
*/
KeypadButton::~KeypadButton()
{
    delete m_label;
    m_label = NULL;
}

/*!
    Updates the state of a keypad when a button's state changes.
*/
void KeypadButton::updateState(bool)
{
    m_keypad->invalidate(State);
}

/*!
    Sends a keypress event when the button is tapped.
*/
void KeypadButton::activate()
{
    Window * const window = Item::window();

    if (window) {
        window->playHaptic(Window::KeyPressEffect);
    }

    if (m_keypad && m_keypad->m_keyPress) {
        m_keypad->m_keyPress(m_code, m_character);
    } else if (window) {
        window->keyPress(m_code, m_character);
    }
}

/*!
    \class Sailfish::MinUi::KeypadButtonTemplate
    \brief A keypad button with an icon or label decoration.
*/


/*!
    Constructs a button member of a \a keypad for a given \a key \a code and ASCII \a character
    combination.

    The button decoration will be initialized with the given a icon or label graphic \a name.
*/
template <typename Decoration>
KeypadButtonTemplate<Decoration>::KeypadButtonTemplate(const char *name, int code, char character, Keypad *parent, const char *label)
    : KeypadButton(code, character, parent, label)
    , m_decoration(name, this)
{
}

/*!
    Destroys a keypad button.
*/
template <typename Decoration>
KeypadButtonTemplate<Decoration>::~KeypadButtonTemplate()
{
}

/*!
    Positions the button decoration.
*/
template <typename Decoration>
void KeypadButtonTemplate<Decoration>::layout()
{
    m_decoration.centerIn(*this);
}

template class KeypadButtonTemplate<Icon>;
template class KeypadButtonTemplate<Label>;

/*!
    \class Sailfish::MinUi::Keypad
    \brief A numeric input keypad.
*/

/*!
    Constructs a new keypad.

    If a \a parent argument is supplied the new item will be appended as a child of that item.
*/
Keypad::Keypad(Item *parent)
    : Keypad(nullptr, nullptr, parent)
{
}

/*!
    Constructs a new keypad with the given \a acceptText and \a cancelText button labels.

    If a \a parent argument is supplied the new item will be appended as a child of that item.
*/
Keypad::Keypad(const char *acceptText, const char *cancelText, Item *parent)
    : ResizeableItem(parent)
    , m_cancelButton(nullptr)
    , m_cancelIconButton(nullptr)
    , m_acceptButton(nullptr)
    , m_acceptIconButton(nullptr)
{
    if (cancelText) {
        m_cancelButton = new KeypadButtonTemplate<Label>(cancelText, KEY_ESC, '\0', this);
    } else {
        m_cancelIconButton = new KeypadButtonTemplate<Icon>("icon-m-cancel", KEY_ESC, '\0', this);
    }

    if (acceptText) {
        m_acceptButton = new KeypadButtonTemplate<Label>(acceptText, KEY_ENTER, '\0', this);
    } else {
        m_acceptIconButton = new KeypadButtonTemplate<Icon>("icon-m-accept", KEY_ENTER, '\0', this);
    }

    setItemFlags(NotifyOnInputFocusChanges);
    resize((theme.itemSizeHuge * 3) + (4 * theme.paddingLarge),
            4 * (theme.sizeCategory >= Theme::Large ? theme.itemSizeExtraLarge : theme.itemSizeLarge));
}

/*!
    Destroys a keypad.
*/
Keypad::~Keypad()
{
    delete m_cancelButton;
    m_cancelButton = nullptr;

    delete m_cancelIconButton;
    m_cancelIconButton = nullptr;

    delete m_acceptButton;
    m_acceptButton = nullptr;

    delete m_acceptIconButton;
    m_acceptIconButton = nullptr;
}

/*!
    \fn Sailfish::MinUi::Keypad::palette

    Returns the color palette for a keypad.
*/

/*!
    Sets the color \a palette of a keypad.
*/
void Keypad::setPalette(const Palette &palette)
{
    m_palette = palette;
    invalidate(State);
}

bool Keypad::isAcceptEnabled() const
{
    return acceptButton()->isEnabled();
}

void Keypad::setAcceptEnabled(bool enabled)
{
    acceptButton()->setEnabled(enabled);
}

bool Keypad::isAcceptVisible() const
{
    return acceptButton()->isVisible();
}

void Keypad::setAcceptVisible(bool visible)
{
    acceptButton()->setVisible(visible);
}

bool Keypad::isCancelEnabled() const
{
    return cancelButton()->isEnabled();
}

void Keypad::setCancelEnabled(bool enabled)
{
    cancelButton()->setEnabled(enabled);
}

bool Keypad::isCancelVisible() const
{
    return cancelButton()->isVisible();
}

void Keypad::setCancelVisible(bool visible)
{
    cancelButton()->setVisible(visible);
}

/*!
    Sets a \a callback for key press events from the keypad.

    If no callback is set the event will be delivered to the current window input focus item.
*/
void Keypad::onKeyPress(const std::function<void(int code, char character)> &callback)
{
    m_keyPress = callback;
    invalidate(State);
}

/*!
    Updates the colors of the buttons when one of the button's states changes or the \a enabled
    state of a parent changes.
*/
void Keypad::updateState(bool enabled)
{
    m_buttonContainer.setEnabled((m_keyPress || (window() && window()->inputFocusItem())));

    enabled = enabled && m_buttonContainer.isEnabled();

    updateButtonState(&m_button0, enabled);
    updateButtonState(&m_button1, enabled);
    updateButtonState(&m_button2, enabled);
    updateButtonState(&m_button3, enabled);
    updateButtonState(&m_button4, enabled);
    updateButtonState(&m_button5, enabled);
    updateButtonState(&m_button6, enabled);
    updateButtonState(&m_button7, enabled);
    updateButtonState(&m_button8, enabled);
    updateButtonState(&m_button9, enabled);
    updateButtonState(acceptButton(), enabled);
    updateButtonState(cancelButton(), enabled);
}

/*!
    Updates the state of an individual \a button. If the keypad is not enabled or no item has
    input focus \a enabled will be false.
*/
void Keypad::updateButtonState(KeypadButton *button, bool enabled) const
{
    if (button->isPressed()) {
        button->setColor(m_palette.pressed);
    } else if (!enabled || !button->isEnabled()) {
        button->setColor(m_palette.disabled);
    } else if (button->hasKeyFocus()) {
        button->setColor(m_palette.selected);
    } else {
        button->setColor(m_palette.normal);
    }
}

KeypadButton *Keypad::cancelButton() const
{
    if (m_cancelButton) {
        return m_cancelButton;
    }
    return m_cancelIconButton;
}

KeypadButton *Keypad::acceptButton() const
{
    if (m_acceptButton) {
        return m_acceptButton;
    }
    return m_acceptIconButton;
}

/*!
    Positions the keypad buttons.
*/
void Keypad::layout()
{
    m_buttonContainer.fill(*this);

    m_button2.centerBetween(m_buttonContainer, Left, m_buttonContainer, Right);
    m_button2.align(Top, m_buttonContainer, Top);

    m_button1.align(Right, m_button2, Left, theme.paddingLarge);
    m_button1.align(Top, m_button2, Top);

    m_button3.align(Left, m_button2, Right, -theme.paddingLarge);
    m_button3.align(Top, m_button2, Top);

    m_button4.align(Left, m_button1, Left);
    m_button4.align(Top, m_button1, Bottom);

    m_button5.align(Left, m_button2, Left);
    m_button5.align(Top, m_button4, Top);

    m_button6.align(Left, m_button3, Left);
    m_button6.align(Top, m_button4, Top);

    m_button7.align(Left, m_button1, Left);
    m_button7.align(Top, m_button4, Bottom);

    m_button8.align(Left, m_button2, Left);
    m_button8.align(Top, m_button7, Top);

    m_button9.align(Left, m_button3, Left);
    m_button9.align(Top, m_button7, Top);

    cancelButton()->align(Left, m_button1, Left);
    cancelButton()->align(Top, m_button7, Bottom);

    m_button0.align(Left, m_button2, Left);
    m_button0.align(Top, *cancelButton(), Top);

    acceptButton()->align(Left, m_button3, Left);
    acceptButton()->align(Top, *cancelButton(), Top);
}

void Keypad::setAcceptText(const char *acceptText)
{
    delete m_acceptIconButton;
    m_acceptIconButton = nullptr;

    delete m_acceptButton;
    m_acceptButton = nullptr;

    if (acceptText) {
        m_acceptButton = new KeypadButtonTemplate<Label>(acceptText, KEY_ENTER, '\0', this);
    } else {
        m_acceptIconButton = new KeypadButtonTemplate<Icon>("icon-m-accept", KEY_ENTER, '\0', this);
    }

    layout();
}

void Keypad::setCancelText(const char *cancelText)
{
    delete m_cancelIconButton;
    m_cancelIconButton = nullptr;

    delete m_cancelButton;
    m_cancelButton = nullptr;

    if (cancelText) {
        m_cancelButton = new KeypadButtonTemplate<Label>(cancelText, KEY_ESC, '\0', this);
    } else {
        m_cancelIconButton = new KeypadButtonTemplate<Icon>("icon-m-cancel", KEY_ESC, '\0', this);
    }

    layout();
}

}}
