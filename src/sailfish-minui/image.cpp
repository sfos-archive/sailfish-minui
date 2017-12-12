/****************************************************************************
**
** Copyright (C) 2017 Jolla Ltd.
** Contact: Andrew den Exter <andrew.den.exter@jolla.com>
**
****************************************************************************/

#include "image.h"

#include <iostream>

namespace Sailfish { namespace MinUi {
/*!
    \class Sailfish::MinUi::Image
    \brief An RGB image.
*/

/*!
    Constructs an image from a graphic resource identified by \a name.

    If a \a parent argument is supplied the new item will be appended as a child of that item.
*/
Image::Image(const char *name, Item *parent)
    : Item(parent)
{
    const int result = res_create_display_surface(name, &m_image);
    if (result != 0) {
        std::cerr << "Failed to load image " << name << " " << result << std::endl;
    }

    resize(gr_get_width(m_image), gr_get_height(m_image));
}

/*!
    Destroys an image.
*/
Image::~Image()
{
    res_free_surface(m_image);
}

/*!
    Draws an icon at the absolute position x, y.

    The \a opacity argument is ignored.
*/
void Image::draw(int x, int y, double opacity)
{
    (void)opacity;

    if (m_image) {
        gr_blit_rgb(m_image, 0, 0, width(), height(), x, y);
    }
}

}}
