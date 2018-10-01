/****************************************************************************
**
** Copyright (C) 2017 Jolla Ltd.
** Contact: Andrew den Exter <andrew.den.exter@jolla.com>
**
****************************************************************************/

#include "textfield.h"

namespace Sailfish { namespace MinUi {

/*!
    \class Sailfish::MinUi::TextFieldTemplate
    \brief A decorated text input field based on either a TextInput or PasswordInput.
*/

/*!
    Constructs a new text field with optional \a placeholder text.

    If a \a parent argument is supplied the new item will be appended as a child of that item.
*/
template <typename Input>
TextFieldTemplate<Input>::TextFieldTemplate(const char *placeholder, Item *parent)
    : ResizeableItem(parent)
    , m_placeholder(placeholder, this)
{
    m_input.setInputFocusOnPress(false);
    setCanActivate(true);
    setKeyFocusOnPress(true);
    setAcceptsKeyFocus(true);
    setInputFocusOnPress(true);

    resize(std::max(theme.buttonWidthMedium, m_placeholder.width()), theme.itemSizeMedium);

    m_backspace.onActivated([this]() {
        if (Window *window = this->window()) {
            window->playHaptic(Window::KeyPressEffect);
        }
        m_input.backspace();
    });
}

/*!
    Destroys a text field.
*/
template <typename Input>
TextFieldTemplate<Input>::~TextFieldTemplate()
{
}

/*!
    \fn Sailfish::MinUi::TextFieldTemplate::palette

    Returns the color palette for a text field.
*/

/*!
    Sets the color \a palette of a text field.
*/
template <typename Input>
void TextFieldTemplate<Input>::setPalette(const Palette &palette)
{
    m_backspace.setPalette(palette);
    invalidate(State);
}

/*!
    \fn Sailfish::MinUi::TextFieldTemplate::horizontalAlignment

    Returns the horizontal display alignment of a text field.
*/

/*!
    Sets the horizontal display \a alignment of a text field.
*/
template <typename Input>
void TextFieldTemplate<Input>::setHorizontalAlignment(HorizontalAlignment alignment)
{
    m_input.setHorizontalAlignment(alignment);
    invalidate(Layout);
}

/*!
    Gives the text field input focus when it is clicked.
*/
template <typename Input>
void TextFieldTemplate<Input>::activate()
{
    if (Window * const window = Item::window()) {
        window->setInputFocusItem(&m_input);
    }
}

/*!
    Positions the child items of a text field.
*/
template <typename Input>
void TextFieldTemplate<Input>::layout()
{
    m_backspace.align(Right, *this, Right);
    m_backspace.centerBetween(*this, Top, *this, Bottom);

    m_input.horizontalFill(*this);
    m_input.centerBetween(*this, Top, *this, Bottom);

    m_input.setLeftMargin(theme.paddingMedium);
    m_input.setRightMargin(width() - m_backspace.x() + theme.paddingMedium);

    switch (m_input.horizontalAlignment()) {
    case HorizontalAlignment::Left:
        m_placeholder.align(Left, *this, Left, theme.paddingMedium);
        break;
    case HorizontalAlignment::Center:
        m_placeholder.centerBetween(*this, Left, *this, Right);
        break;
    case HorizontalAlignment::Right:
        m_placeholder.align(Right, m_backspace, Left, -theme.paddingMedium);
        break;
    }
    m_placeholder.centerBetween(*this, Top, *this, Bottom);

    m_underline.setHeight(theme.scale(1));
    m_underline.align(Bottom, m_input, Bottom, theme.paddingSmall);
    m_underline.horizontalFill(*this, theme.paddingMedium);
}

/*!
    Updates the colors of a text field when its state or a parent's \a enabled state changes.
*/
template <typename Input>
void TextFieldTemplate<Input>::updateState(bool enabled)
{
    const bool hasText = m_input.text().length() > 0;
    m_backspace.setVisible(hasText);
    m_placeholder.setVisible(!hasText && !m_input.hasInputFocus());

    const Palette palette = m_backspace.palette();

    if (isPressed() || m_input.hasInputFocus()) {
        setForegroundColor(palette.pressed);
    } else if (!enabled) {
        setForegroundColor(palette.disabled);
    } else if (hasKeyFocus()) {
        setForegroundColor(palette.selected);
    } else {
        m_placeholder.setColor(palette.disabled);
        m_input.setColor(palette.normal);
        m_underline.setColor(palette.selected);
    }
}

/*!
    Sets the \a color of foreground child items in a text field.
*/
template <typename Input>
void TextFieldTemplate<Input>::setForegroundColor(Color color)
{
    m_placeholder.setColor(color);
    m_input.setColor(color);
    m_underline.setColor(color);
}

template class TextFieldTemplate<TextInput>;

/*!
    \class Sailfish::MinUi::TextField
    \brief A decorated text input field.
*/

/*!
    \fn TextField::TextField(const char *placeholder, Item *parent)

    Constructs a new text input field.

    If a \a parent argument is supplied the new item will be appended as a child of that item.
*/

/*!
    Constructs a new text input field.

    If no text is entered into the field the \a placeholder string will be displayed instead.

    If a \a parent argument is supplied the new item will be appended as a child of that item.
*/
TextField::TextField(const char *placeholder, Item *parent)
    : TextFieldTemplate<TextInput>(placeholder, parent)
{
}

/*!
    Destroys a text field.
*/
TextField::~TextField()
{
}

template class TextFieldTemplate<PasswordInput>;

/*!
    \class Sailfish::MinUi::TextField
    \brief A decorated password input field.
*/

/*!
    \fn PasswordField::PasswordField(const char *placeholder)

    Constructs a new password input field.

    If a \a parent argument is supplied the new item will be appended as a child of that item.
*/

/*!
    Constructs a new password input field.

    If no text is entered into the field the \a placeholder string will be displayed instead.

    If a \a parent argument is supplied the new item will be appended as a child of that item.
*/
PasswordField::PasswordField(const char *placeholder, Item *parent)
    : TextFieldTemplate<PasswordInput>(placeholder, parent)
{
    setBorderVisible(false);
    setHorizontalAlignment(HorizontalAlignment::Center);
}

/*!
    Destroys a password field.
*/
PasswordField::~PasswordField()
{
}

}}
