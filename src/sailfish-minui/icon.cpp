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

#include "icon.h"

#include "logging.h"

namespace Sailfish { namespace MinUi {

/*!
    \class Sailfish::MinUi::Icon
    \brief A single color image.
*/

/*!
    Constructs an icon from a graphic resource identified by \a name.

    If a \a parent argument is supplied the new item will be appended as a child of that item.
*/
Icon::Icon(const char *name, Item *parent)
    : Item(parent)
{
    const int result = res_create_alpha_surface(name, &m_icon);
    if (result != 0) {
        log_err("Failed to load icon " << name << " " << result);
    }

    resize(gr_get_width(m_icon), gr_get_height(m_icon));
}

/*!
    Destroys an icon.
*/
Icon::~Icon()
{
    res_free_surface(m_icon);
}

/*!
    \fn Sailfish::MinUi::Icon::color() const

    Returns the color of an icon.
*/

/*!
    Sets the \a color of an icon.
*/
void Icon::setColor(Color color)
{
    m_color = color;
    invalidate(Draw);
}

/*!
    Draws an icon at the absolute position x, y, with the given accumulative \a opacity.
*/
void Icon::draw(int x, int y, double opacity)
{
    if (m_icon) {
        const uint8_t alpha = m_color.a * opacity;
        if (alpha != 0) {
            gr_color(m_color.r, m_color.g, m_color.b, alpha);
            gr_texticon(x, y, m_icon);
        }
    }
}

}}
