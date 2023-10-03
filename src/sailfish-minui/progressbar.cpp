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

#include "progressbar.h"

namespace Sailfish { namespace MinUi {

/*!
    \class Sailfish::MinUi::ProgressBar
    \brief An item which display progress.
*/

/*!
    Constructs a new progress bar.

    If a \a parent argument is supplied the new item will be appended as a child of that item.
*/
ProgressBar::ProgressBar(Item *parent)
    : ResizeableItem(parent)
{
    resize(theme.buttonWidthLarge, theme.itemSizeMedium);
    m_background.setHeight(theme.paddingMedium);
    m_foreground.setHeight(theme.paddingMedium);

    m_background.setColor({ 105, 105, 105, 255 });
    m_foreground.setColor({ 255, 255, 255, 255 });

    invalidate(Layout);
}

/*!
    Destroys a progress bar.
*/
ProgressBar::~ProgressBar()
{
}

/*!
    \fn Sailfish::MinUi::value() const

    Returns the displayed value.
*/

/*!
    Sets the displayed \a value.

    This is a normalized value in the range 0 to 1.
*/
void ProgressBar::setValue(double value)
{
    value = std::min(1., std::max(0., value));
    if (m_value != value) {
        m_value = value;
        invalidate(Layout);
    }
}

/*!
    Positions the progress bar's child items.
*/
void ProgressBar::layout()
{
    m_background.centerBetween(*this, Top, *this, Bottom);
    m_foreground.align(Top, m_background, Top);

    const int totalWidth = width();
    const int progressWidth = totalWidth * m_value;

    m_foreground.setX(0);
    m_foreground.setWidth(progressWidth);
    m_background.setX(progressWidth);
    m_background.setWidth(totalWidth - progressWidth);
}

}}
