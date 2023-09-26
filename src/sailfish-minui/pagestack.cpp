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

#include "pagestack.h"

namespace Sailfish { namespace MinUi {

/*!
    \class Sailfish::MinUi::Page
    \brief A page in a page stack.
*/

/*!
    Constructs a new page member of a \a stack encapsulating a \a contentItem.
*/
Page::Page(Item *contentItem, PageStack *stack)
    : Item(stack)
    , m_contentItem(contentItem)
    , m_stack(stack)
{
}

/*!
    Destroys a page.
*/
Page::~Page()
{
}

/*!
    Removes a page from its page stack. This will delete the page.
*/
void Page::remove()
{
    const bool wasTop = m_stack->m_pages.last() == this;

    m_stack->invalidate(Layout);
    m_stackNode.erase();
    if (m_removed) {
        m_removed();
    }

    if (wasTop && !m_stack->m_pages.isEmpty()) {
        Page *newTop = m_stack->m_pages.last();
        newTop->setVisible(true);

        if (Window * const window = Item::window()) {
            Item * const focusItem = window->inputFocusItem();
            if (!focusItem || isAncestorOf(focusItem)) {
                window->setInputFocusItem(newTop);
            }
        }
    }

    delete this;
}

/*!
    Sets a \a callback which will be invoked when a page is removed from its stack.
*/
void Page::onRemoved(const std::function<void()> &callback)
{
    m_removed = callback;
}

/*!
    \class Sailfish::MinUi::PageHeader
    \brief A page title with an optional back button.
*/

/*!
    Constructs a new page header which displays the given \a label.

    If a \a parent argument is supplied the new item will be appended as a child of that item.
*/
PageHeader::PageHeader(const char *label, Item *parent)
    : PageHeader(nullptr, label, parent)
{
}

/*!
    Constructs a new page header which displays the given \a label and back button \a icon.

    If a \a parent argument is supplied the new item will be appended as a child of that item.
*/
PageHeader::PageHeader(const char *icon, const char *label, Item *parent)
    : Item(parent)
    , m_label(label, this)
    , m_backButton(icon, this)
{
    m_label.setColor({ 127, 223, 255, 255 });

    m_backButton.setEnabled(icon != nullptr);

    resize(parent
                ? parent->width()
                : theme.horizontalPageMargin + m_backButton.width() + theme.paddingMedium + m_label.width(),
            theme.itemSizeLarge);
}

/*!
    Destroys a page header.
*/
PageHeader::~PageHeader()
{

}

/*!
    Sets the \a callback which will be invoked when the header back button is activated.
*/
void PageHeader::onBackActivated(const std::function<void()> &callback)
{
    m_backButton.onActivated(callback);
}

/*!
    Positions the header label on the right and the back button on the left.
*/
void PageHeader::layout()
{
    if (Item * const parent = Item::parent()) {
        setWidth(parent->width());
    }

    m_backButton.centerBetween(*this, Top, *this, Bottom);
    m_backButton.align(Left, *this, Left, theme.horizontalPageMargin - theme.paddingMedium); // counter padding in button.

    m_label.align(Right, *this, Right, -theme.horizontalPageMargin);
    m_label.centerBetween(*this, Top, *this, Bottom);
}

/*!
    \class Sailfish::MinUi::PageStack
    \brief An item containing a stack of pages of which only the top-most is visible.
*/

/*!
    Constructs a new page stack.

    If a \a parent argument is supplied the new item will be appended as a child of that item.
*/
PageStack::PageStack(Item *parent)
    : ResizeableItem(parent)
{
}

/*!
    Destroys a page stack.
*/
PageStack::~PageStack()
{
    while (m_pages.isEmpty()) {
        delete m_pages.first();
    }
}

/*!
    \fn Sailfish::MinUi::PageStack::pushNew()

    Pushes a new page onto the stack with a content item constructed from the template
    type.  Any arguments passed to this function will be forwarded to the constructor of
    the content item.

    Returns a pointer to the newly constructed page.
*/

/*!
    Pushes a new \a content item onto a stack. Returns a new page for the content.
*/
Page *PageStack::push(Item *content)
{
    return appendPage(new Page(content, this));
}

/*!
    Removes the top-most item from the stack.
*/
void PageStack::pop()
{
    if (!m_pages.isEmpty()) {
        m_pages.last()->remove();
    }
}

/*!
    Removes a content \a item from the stack.
*/
void PageStack::remove(Item *item)
{
    for (Page &page : m_pages) {
        if (page.contentItem() == item) {
            page.remove();
            return;
        }
    }
}

/*!
    Resizes the child pages of the stack to its size.
*/
void PageStack::layout()
{
    if (!m_pages.isEmpty()) {
        m_pages.last()->resize(width(), height());
    }
}

/*!
    Appends a \a page to the top of stack. Returns a pointer to the page argument.
*/
Page *PageStack::appendPage(Page *page)
{
    appendChild(page);

    if (!m_pages.isEmpty()) {
        m_pages.last()->setVisible(false);
    }

    m_pages.append(page);

    invalidate(Layout);

    if (Window * const window = Item::window()) {
        if (!window->inputFocusItem()) {
            window->setInputFocusItem(page);
        }
    }

    return page;
}

}}
