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

#include "label.h"

#include "logging.h"

namespace Sailfish { namespace MinUi {

/*!
    \class Sailfish::MinUi::Label
    \brief A localized label image.
*/

/*!
    Constructs a label from a graphic resource identified by \a name.

    If a \a parent argument is supplied the new item will be appended as a child of that item.
*/
Label::Label(const char *name, Item *parent)
    : Item(parent)
{
    if (name) {
        const int result = res_create_localized_alpha_surface(name, locale(), &m_text);
        if (result != 0) {
            log_err("Failed to load label " << name << " " << result);
        }

        resize(gr_get_width(m_text), gr_get_height(m_text));
    }
}

/*!
    Destroys a label.
*/
Label::~Label()
{
    res_free_surface(m_text);
}

/*!
    \fn Sailfish::MinUi::Icon::color() const

    Returns the color of a label.
*/

/*!
    Sets the \a color of a label.
*/
void Label::setColor(Color color)
{
    m_color = color;
    invalidate(Draw);
}

/*!
    Draws an icon at the absolute position x, y, with the given accumulative \a opacity.
*/
void Label::draw(int x, int y, double opacity)
{
    if (m_text) {
        const uint8_t alpha = m_color.a * opacity;
        if (alpha != 0) {
            gr_color(m_color.r, m_color.g, m_color.b, alpha);
            gr_texticon(x, y, m_text);
        }
    }
}

/*!
    \class Sailfish::MinUi::LiteralLabel
    \brief unlocalized label text.
*/
LiteralLabel::LiteralLabel(std::string text, Item* parent):
    Item(parent),
    m_text(text)
{
    gr_font_size(&m_fontWidth, &m_fontHeight);
}

void LiteralLabel::setColor(Color color)
{
    m_color = color;
    invalidate(Draw);
}

void LiteralLabel::setText(std::string text)
{
    m_text = text;
    invalidate(Draw);
}

/*!
    Draws text with text center in x, with the given accumulative \a opacity.
*/
void LiteralLabel::draw(int x, int y, double opacity)
{
    const uint8_t alpha = m_color.a * opacity;
    if (alpha == 0)
        return;

    gr_color(m_color.r, m_color.g, m_color.b, (m_color.a * opacity));

    // output the text tighter. width need what happens to look good for some known usage
    float charDistance = m_fontWidth * 0.75;
    int width = charDistance * m_text.size();
    int start_x = x - width/2;

    char input[2] = "\0";
    for (char c : m_text) {
        input[0] = c;
        gr_text(start_x, y - (m_fontHeight/2), input, 0);
        start_x += charDistance;
    }
}

}}
