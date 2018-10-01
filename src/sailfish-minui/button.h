/****************************************************************************
**
** Copyright (C) 2017 Jolla Ltd.
** Contact: Andrew den Exter <andrew.den.exter@jolla.com>
**
****************************************************************************/

#ifndef SAILFISH_MINUI_BUTTON_H
#define SAILFISH_MINUI_BUTTON_H

#include <sailfish-minui/icon.h>
#include <sailfish-minui/label.h>

namespace Sailfish { namespace MinUi {

template <typename Decoration>
class ButtonTemplate : public ActivatableItem
{
public:
    explicit ButtonTemplate(const char *name, Item *parent = nullptr);
    ~ButtonTemplate();

    Palette palette() const { return m_palette; }
    void setPalette(const Palette &palette);

protected:
    void layout() override;
    void updateState(bool enabled) override;

    Decoration m_decoration;

private:
    Palette m_palette;
};

class Button : public ButtonTemplate<Label>
{
public:
    Button(const char *name, Item *parent = nullptr);
    ~Button();
};

class IconButton : public ButtonTemplate<Icon>
{
public:
    IconButton(const char *name, Item *parent = nullptr);
    ~IconButton();
};

}}

#endif
