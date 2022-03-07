/*
 * Copyright (c) 2017-2019 Jolla Ltd.
 *
 * License: Proprietary
 */

#ifndef SAILFISH_MINUI_IMAGE_H
#define SAILFISH_MINUI_IMAGE_H

#include <sailfish-minui/item.h>

namespace Sailfish { namespace MinUi {

class Image : public Item
{
public:
    explicit Image(const char *name, Item *parent);
    ~Image();

    bool isValid() const { return m_image; }

protected:
    void draw(int x, int y, double opacity) override;

private:
    gr_surface m_image = nullptr;
};

}}

#endif
