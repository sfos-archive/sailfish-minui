/****************************************************************************
**
** Copyright (C) 2017 Jolla Ltd.
** Contact: Andrew den Exter <andrew.den.exter@jolla.com>
**
****************************************************************************/

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

}}
