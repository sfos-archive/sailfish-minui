/****************************************************************************
**
** Copyright (C) 2017 Jolla Ltd.
** Contact: Andrew den Exter <andrew.den.exter@jolla.com>
**
****************************************************************************/

#ifndef SAILFISH_MINUI_MENU_H
#define SAILFISH_MINUI_MENU_H

#include <sailfish-minui/button.h>
#include <sailfish-minui/label.h>

namespace Sailfish { namespace MinUi {

class MenuItem : public IconButton
{
public:
    explicit MenuItem(const char *label, Item *parent = nullptr);
    MenuItem(const char *icon, const char *label, Item *parent = nullptr);
    ~MenuItem();

protected:
    void updateState(bool enabled) override;
    void layout() override;

private:
    Label m_label;
};

class Menu : public ContainerItem
{
public:
    explicit Menu(Item *parent = nullptr);
    ~Menu();

protected:
    void layout();
};

}}

#endif
