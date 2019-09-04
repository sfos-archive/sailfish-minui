/*
 * Copyright (C) 2017-2019 Jolla Ltd.
 *
 * License: Proprietary
 */

#ifndef SAILFISH_MINUI_RECTANGLE_H
#define SAILFISH_MINUI_RECTANGLE_H

#include <sailfish-minui/item.h>

namespace Sailfish { namespace MinUi {

class Rectangle : public ResizeableItem
{
public:
    explicit Rectangle(Item *parent);
    ~Rectangle();

    Color color() const { return m_color; }
    void setColor(Color color);

protected:
    void draw(int x, int y, double opacity) override;

private:
    Color m_color;
};

}}

#endif
