/*
 * Copyright (C) 2017-2019 Jolla Ltd.
 *
 * License: Proprietary
 */

#ifndef SAILFISH_MINUI_PROGRESSBAR_H
#define SAILFISH_MINUI_PROGRESSBAR_H

#include <sailfish-minui/rectangle.h>

namespace Sailfish { namespace MinUi {

class ProgressBar : public ResizeableItem
{
public:
    explicit ProgressBar(Item *parent);
    ~ProgressBar();

    double value() const { return m_value; }
    void setValue(double value);

protected:
    void layout();

private:
    Rectangle m_background { this };
    Rectangle m_foreground { this };
    double m_value = 0.;
};

}}

#endif
