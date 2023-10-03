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

#include "button.h"

namespace Sailfish { namespace MinUi {

/*!
    \class Sailfish::MinUi::ButtonTemplate
    \brief An activatable button type with either a label or icon decoration.
*/

/*!
    Constructs a new button with a label or icon graphic identified by \a name.

    If a \a parent argument is supplied the new item will be appended as a child of that item.
*/
template <typename Decoration>
ButtonTemplate<Decoration>::ButtonTemplate(const char *name, Item *parent)
    : ActivatableItem(parent)
    , m_decoration(name, this)
{
    invalidate(State | Layout);

    resize(m_decoration.width() + (2 * theme.paddingMedium), m_decoration.height() + (2 * theme.paddingMedium));
}

/*!
    Destroys a button.
*/
template <typename Decoration>
ButtonTemplate<Decoration>::~ButtonTemplate()
{
}

/*!
    \fn Sailfish::MinUi::ButtonTemplate::palette

    Returns the color palette for a button.
*/

/*!
    Sets the color \a palette of a button.
*/
template <typename Decoration>
void ButtonTemplate<Decoration>::setPalette(const Palette &palette)
{
    m_palette = palette;
    invalidate(State);
}

/*!
    Positions the button decoration.
*/
template <typename Decoration>
void ButtonTemplate<Decoration>::layout()
{
    m_decoration.centerIn(*this);
}

/*!
    Updates the color of the button decoration when its state or a parent's \a enabled
    state changes.
*/
template <typename Decoration>
void ButtonTemplate<Decoration>::updateState(bool enabled)
{
    if (isPressed()) {
        m_decoration.setColor(m_palette.pressed);
    } else if (!enabled) {
        m_decoration.setColor(m_palette.disabled);
    } else if (hasKeyFocus()) {
        m_decoration.setColor(m_palette.selected);
    } else {
        m_decoration.setColor(m_palette.normal);
    }
}

template class ButtonTemplate<Label>;

/*!
    \class Sailfish::MinUi::Button
    \brief An activatable label button
*/

/*!
    Contructs a button with the label graphic identified by \a name.

    If a \a parent argument is supplied the new item will be appended as a child of that item.
*/
Button::Button(const char *name, Item *parent)
    : ButtonTemplate<Label>(name, parent)
{
}

/*!
    Destroys a button.
*/
Button::~Button()
{
}

template class ButtonTemplate<Icon>;

/*!
    \class Sailfish::MinUi::Button
    \brief An activatable icon button
*/

/*!
    Contructs a button with the icon graphic identified by \a name.

    If a \a parent argument is supplied the new item will be appended as a child of that item.
*/
IconButton::IconButton(const char *name, Item *parent)
    : ButtonTemplate<Icon>(name, parent)
{
}

/*!
    Destroys a button.
*/
IconButton::~IconButton()
{
}

}}
