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

#include "image.h"

#include "logging.h"

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
        log_err("Failed to load image " << name << " " << result);
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
