/****************************************************************************
**
** Copyright (C) 2017 Jolla Ltd.
** Contact: Andrew den Exter <andrew.den.exter@jolla.com>
**
****************************************************************************/

#ifndef SAILFISH_MINUI_LABEL_H
#define SAILFISH_MINUI_LABEL_H

#include <sailfish-minui/item.h>

namespace Sailfish { namespace MinUi {

class Label : public Item
{
public:
    explicit Label(const char *name, Item *parent = nullptr);
    ~Label();

    bool isValid() const { return m_text; }

    Color color() const { return m_color; }
    void setColor(Color color);

protected:
    void draw(int x, int y, double opacity) override;

private:
    gr_surface m_text = nullptr;
    Color m_color;
};

}}

#endif
