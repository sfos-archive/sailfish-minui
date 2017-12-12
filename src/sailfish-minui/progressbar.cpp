/****************************************************************************
**
** Copyright (C) 2017 Jolla Ltd.
** Contact: Andrew den Exter <andrew.den.exter@jolla.com>
**
****************************************************************************/

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
