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

#include "menu.h"

namespace Sailfish { namespace MinUi {

/*!
    \class Sailfish::MinUi::MenuItem
    \brief An activatable menu item with a label and an optional icon.
*/

/*!
    Constructs a menu item with a label graphic identified by \a label.

    If a \a parent argument is supplied the new item will be appended as a child of that item.
*/
MenuItem::MenuItem(const char *label, Item *parent)
    : MenuItem(nullptr, label, parent)
{
}

/*!
    Constructs a menu item with an icon graphic identified by \a icon and a label graphic identified
    by \a label.

    If a \a parent argument is supplied the new item will be appended as a child of that item.
*/
MenuItem::MenuItem(const char *icon, const char *label, Item *parent)
    : IconButton(icon, parent)
    , m_label(label, this)
{
    resize(parent
                ? parent->width()
                : theme.horizontalPageMargin + m_decoration.width() + theme.paddingMedium + m_label.width(),
            theme.itemSizeMedium);
}

/*!
    Destroys a menu item.
*/
MenuItem::~MenuItem()
{
}

/*!
    Updates the colors of the icon and label when the state of the item or the \a enabled state
    of a parent changes.
*/
void MenuItem::updateState(bool enabled)
{
    IconButton::updateState(enabled);

    m_label.setColor(m_decoration.color());
}

/*!
    Positions the icon and label of the menu item.
*/
void MenuItem::layout()
{
    if (Item * const parent = Item::parent()) {
        setWidth(parent->width());
    }

    if (m_decoration.isValid()) {
        m_decoration.align(Left, *this, Left, theme.horizontalPageMargin);
        m_decoration.centerBetween(*this, Top, *this, Bottom);

        m_label.align(Left, m_decoration, Right, theme.paddingMedium);
    } else {
        m_label.align(Left, *this, Left, theme.horizontalPageMargin);
    }

    m_label.centerBetween(*this, Top, *this, Bottom);
}

/*!
    \class Sailfish::MinUi::Menu
    \brief A menu item which stacks its children vertically.

    If a \a parent argument is supplied the new item will be appended as a child of that item.
*/
Menu::Menu(Item *parent)
    : ContainerItem(parent)
{
}

/*!
    Destroys a menu.
*/
Menu::~Menu()
{
}

/*!
    Positions the menu's children in a vertical stack.
*/
void Menu::layout()
{
    Item *previous = nullptr;
    ChildList &childItems = Item::childItems();
    for (Item &item : childItems) {
        if (!item.isVisible()) {
            continue;
        }

        if (previous) {
            item.align(Top, *previous, Bottom);
        } else {
            item.align(Top, *this, Top);
        }
        previous = &item;
    }
    if (!childItems.isEmpty()) {
        setHeight(childItems.last()->bottom());
    }
}

}}
