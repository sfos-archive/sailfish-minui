/****************************************************************************
**
** Copyright (C) 2017 Jolla Ltd.
** Contact: Andrew den Exter <andrew.den.exter@jolla.com>
**
****************************************************************************/

#include "icon.h"

#include <iostream>

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
        std::cerr << "Failed to load icon " << name << " " << result << std::endl;
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
