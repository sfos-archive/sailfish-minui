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
#include "rectangle.h"

namespace Sailfish { namespace MinUi {

/*!
    \class Sailfish::MinUi::Rectangle
    \brief A solid color rectangle.
*/

/*!
    Constructs a new rectangle.

    If a \a parent argument is supplied the new item will be appended as a child of that item.
*/
Rectangle::Rectangle(Item *parent)
    : ResizeableItem(parent)
{
}

/*!
    Destroys a rectangle.
*/
Rectangle::~Rectangle()
{
}

/*!
    \fn Sailfish::MinUi::Rectangle::color() const

    Returns the color of a rectangle.
*/

/*!
    Sets the \a color of a rectangle.
*/
void Rectangle::setColor(Color color)
{
    m_color = color;
    invalidate(Draw);
}

/*!
    Draws a rectangle at the absolute position x, y, with the given accumulative \a opacity.
*/
void Rectangle::draw(int x, int y, double opacity)
{
    const uint8_t alpha = m_color.a * opacity;
    if (alpha != 0) {
        gr_color(m_color.r, m_color.g, m_color.b, alpha);
        gr_fill(x, y, x + width(), y + height());
    }
}

}}
