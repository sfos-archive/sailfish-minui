/*
 * Copyright (c) 2017-2019 Jolla Ltd.
 *
 * License: Proprietary
 */

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
