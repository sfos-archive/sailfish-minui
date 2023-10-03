/****************************************************************************************
** Copyright (c) 2017 - 2023 Jolla Ltd.
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

#include "textinput.h"
#include "eventloop.h"

#include <linux/input.h>

#include <iostream>

namespace Sailfish { namespace MinUi {

/*!
  ` \class Sailfish::MinUi::TextInput
    \brief An undecorated latin text input field.
*/

/*!
    Constructs a new text input.

    If a \a parent argument is supplied the new item will be appended as a child of that item.
*/
TextInput::TextInput(Item *parent)
    : ResizeableItem(parent)
{
    setAcceptsInputFocus(true);
    setInputFocusOnPress(true);

    gr_font_size(&m_fontWidth, &m_fontHeight);

    setHeight(m_fontHeight);
}

/*!
    Destroys a text input.
*/
TextInput::~TextInput()
{
}

/*!
    \fn Sailfish::MinUi::TextInput::text() const

    Returns the current \a text content of a text input.
*/

/*!
    Sets the \a text content of a text input.
*/
void TextInput::setText(const std::string &text)
{
    if (int(text.length()) <= m_maximumLength) {
        m_text = text;
        textChanged(Assignment);
        invalidate(Draw | State);
    }
}

/*!
    \fn Sailfish::MinUi::TextInput::color() const

    Returns the color of a text input.
*/

/*!
    Sets the \a color of a text input.
*/
void TextInput::setColor(Color color)
{
    m_color = color;
    invalidate(Draw);
}

/*!
    \fn Sailfish::MinUi::TextInput::isBold() const

    Returns whether the text should be displayed in a bold font.
*/

/*!
    Sets whether the text should be displayed in a \a bold font.
*/
void TextInput::setBold(bool bold)
{
    if (m_bold != bold) {
        m_bold = bold;
        invalidate(Draw);
    }
}

/*!
    \fn Sailfish::MinUi::TextInput::maximumLength

    Returns the maximum number of characters that may be entered.
*/

/*!
    Sets the maximum string \a length that may be entered.
*/
void TextInput::setMaximumLength(int length)
{
    m_maximumLength = length;
}

/*!
    HorizontalAlignment TextInput::horizontalAlignment() const

    Returns the horizontal alignment of the text input contents.
*/

/*!
    Sets the horizontal \a alignment of the text input contents.
*/
void TextInput::setHorizontalAlignment(HorizontalAlignment alignment)
{
    m_horizontalAlignment = alignment;
    invalidate(Draw);
}

/*!
    \fn int TextInput::leftMargin() const

    Returns the left display margin of the text input.
*/

/*!
    Sets the \a left display margin of the text input.

    This is the margin from left of the input to the start of left aligned text. This does
    not affect the center position for center aligned text.
*/
void TextInput::setLeftMargin(int margin)
{
    m_leftMargin = margin;
    invalidate(Draw);
}

/*!
    \fn int TextInput::rightMargin() const

    Returns the \a right display margin of the text input.
*/

/*!
    Sets the \a right display margin of the text input.

    This is the margin from right of the input to the start of right aligned text. This does
    not affect the center position for center aligned text.
*/
void TextInput::setRightMargin(int margin)
{
    m_rightMargin = margin;
    invalidate(Draw);
}

/*!
    Removes the last character from the input text.
*/
void TextInput::backspace()
{
    if (m_text.length() > 0) {
        m_text.resize(m_text.length() - 1);
        textChanged(Deletion);
        invalidate(State);
    }
}

void TextInput::onTextChanged(const std::function<void (TextInput::Reason)> &callback)
{
    m_textChanged = callback;
}

/*!
    Sets a \a callback which will be invoked when the text input is accepted by pressing the
    enter key.
*/
void TextInput::onAccepted(const std::function<void(const std::string &text)> &callback)
{
    m_accepted = callback;
}

/*!
    Sets a \a callback which will be invoked when the text input is canceled by pressing the
    escape key.
*/
void TextInput::onCanceled(const std::function<void()> &callback)
{
    m_canceled = callback;
}

/*!
    Transforms input \a text into a string suitable for display and returns it.
*/
std::string TextInput::displayText(const std::string &text)
{
    return text;
}

/*!
    Notifies that the input text has changed and the \a reason it changed.
*/
void TextInput::textChanged(Reason reason)
{
    if (m_textChanged) {
        m_textChanged(reason);
    }
}

/*!
    Draws input text at the absolute position x, y, with the given accumulative \a opacity.
*/
void TextInput::draw(int x, int y, double opacity)
{
    const uint8_t alpha = m_color.a * opacity;
    if (alpha != 0) {
        const int availableWidth = width() - m_leftMargin - m_rightMargin;
        const int characterCount = m_displayText.length();
        const int visibleCharacterCount = availableWidth / m_fontWidth;
        const int fadedCharacterCount = std::min(2, std::max(0, characterCount - visibleCharacterCount));
        const int croppedCharacterCount = std::max(0, characterCount - visibleCharacterCount + fadedCharacterCount);

        if (fadedCharacterCount > 0 || m_horizontalAlignment == HorizontalAlignment::Right) {
            x += m_leftMargin + availableWidth - ((characterCount - croppedCharacterCount) * m_fontWidth);
        } else if (m_horizontalAlignment == HorizontalAlignment::Left) {
            x += m_leftMargin;
        } else {
            x += std::max(m_leftMargin, (width() - (std::min(visibleCharacterCount, characterCount) * m_fontWidth)) / 2);
        }

        gr_color(m_color.r, m_color.g, m_color.b, alpha);
        gr_text(x, y, m_displayText.data() + croppedCharacterCount, m_bold);

        for (int i = 1; i <= fadedCharacterCount; ++i) {
            x -= m_fontWidth;

            gr_color(m_color.r, m_color.g, m_color.b, (alpha * (3 - i)) / 3);

            char fadedText[2] = "\0";
            fadedText[0] = m_displayText[croppedCharacterCount - i];

            gr_text(x, y, fadedText, m_bold);
        }
    }
}

/*!
    Updates the display text.
*/
void TextInput::updateState(bool)
{
    m_displayText = displayText(m_text);
    invalidate(Draw);
    invalidateParent(State);
}

/*!
    Handles a key press event with the given key \a code and associated latin \a character.

    Returns true if the input handled the event.
*/
bool TextInput::keyPress(int code, const char character)
{
    switch (code) {
    case KEY_ENTER:
        textChanged(Accepted);
        if (m_accepted) {
            m_accepted(m_text);
        }
        return true;
    case KEY_ESC:
        textChanged(Canceled);
        if (m_canceled) {
            m_canceled();
        }
        return true;
    case KEY_BACKSPACE:
        backspace();
        return true;
    default:
        if (character >= 0x20 && character <= 0x7e && int(m_text.length() + 1) < m_maximumLength) {
            m_text.push_back(character);
            textChanged(Insertion);
            invalidate(State);
            return true;
        }
    }
    return false;
}

/*!
    \class Sailfish::MinUi::PasswordInput
    \brief An undecorated latin password input field.
*/

/*!
    Constructs a new password input.

    If a \a parent argument is supplied the new item will be appended as a child of that item.
*/
PasswordInput::PasswordInput(Item *parent)
    : TextInput(parent)
{
}

/*!
    Destroys a password input.
*/
PasswordInput::~PasswordInput()
{
    finishEcho();
}

/*!
    \fn Sailfish::MinUi::PasswordInput::echoDelay() const

    Returns the delay in milliseconds before an entered character will be masked.
*/

/*!
    Sets the \a delay in milliseconds before an entered character will be masked.
*/
void PasswordInput::setEchoDelay(int delay)
{
    m_echoDelay = delay;
    finishEcho();
    invalidate(Draw | State);
}

void PasswordInput::setMaskingEnabled(bool enabled)
{
    if (enabled == m_maskingEnabled) {
        return;
    }

    m_maskingEnabled = enabled;
    finishEcho();
    invalidate(Draw | State);
}

/*!
    Masks password \a text and returns the result.
*/
std::string PasswordInput::displayText(const std::string &text)
{
    if (!m_maskingEnabled) {
        return text;
    }

    std::string copy = text;
    const size_t length = m_maskTimerId > 0
            ? (text.length() > 0 ? text.length() - 1 : 0)
            : text.length();
    for (size_t i = 0; i < length; ++i) {
        copy[i] = '*';
    }

    return copy;
}

/*!
    Starts the mask delay timer if the \a reason the text changed was that new text was inserted.
*/
void PasswordInput::textChanged(Reason reason)
{
    finishEcho();

    if (reason == Insertion && m_echoDelay > 0) {
        m_maskTimerId = eventLoop()->createTimer(m_echoDelay, [this]() {
            finishEcho();
            invalidate(State);
        });
    }

    TextInput::textChanged(reason);
}

/*!
    Updates the display text after the mask delay timer expires.
*/
void PasswordInput::finishEcho()
{
    if (m_maskTimerId > 0) {
        eventLoop()->cancelTimer(m_maskTimerId);
        m_maskTimerId = 0;
    }
}

}}
