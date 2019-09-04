/*
 * Copyright (C) 2017-2019 Jolla Ltd.
 *
 * License: Proprietary
 */

#ifndef SAILFISH_MINUI_ICON_H
#define SAILFISH_MINUI_ICON_H

#include <sailfish-minui/item.h>

namespace Sailfish { namespace MinUi {

class Icon : public Item
{
public:
    explicit Icon(const char *name, Item *parent = nullptr);
    ~Icon();

    bool isValid() const { return m_icon; }

    Color color() const { return m_color; }
    void setColor(Color color);

protected:
    void draw(int x, int y, double opacity) override;

private:
    gr_surface m_icon = nullptr;
    Color m_color;
};
}}

#endif
