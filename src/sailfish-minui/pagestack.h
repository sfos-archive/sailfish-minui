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

#ifndef SAILFISH_MINUI_PAGESTACK_H
#define SAILFISH_MINUI_PAGESTACK_H

#include <sailfish-minui/button.h>

namespace Sailfish { namespace MinUi {

class PageStack;

class Page : public Item
{
public:
    ~Page();

    Item *contentItem() const { return m_contentItem; }
    PageStack *pageStack() const { return m_stack; }

    void remove();

    void onRemoved(const std::function<void()> &callback);

private:
    friend class PageStack;

    explicit Page(Item *contentItem, PageStack *stack);

    Item * const m_contentItem;
    PageStack * const m_stack;
    LinkedListNode m_stackNode;
    std::function<void()> m_removed;
};

class PageHeader : public Item
{
public:
    explicit PageHeader(const char *label, Item *parent = nullptr);
    PageHeader(const char *icon, const char *label, Item *parent = nullptr);
    ~PageHeader();

    void onBackActivated(const std::function<void()> &callback);

protected:
    void layout() override;

private:
    Label m_label;
    IconButton m_backButton;
};

class PageStack : public ResizeableItem
{
public:
    PageStack(Item *parent = nullptr);
    ~PageStack();

    Page *push(Item *content);
    template <typename ContentItem, typename... Arguments> Page *pushNew(Arguments... arguments)
    {
        return appendPage(new PageTemplate<ContentItem, Arguments...>(arguments..., this));
    }

    void pop();
    void remove(Item *item);

protected:
    void layout();

private:
    friend class Page;

    template <typename ContentItem, typename... Arguments> class PageTemplate : public Page
    {
    public:
        PageTemplate(Arguments... arguments, PageStack *stack)
            : Page(&m_contentItem, stack)
            , m_contentItem(arguments..., this)
        {
        }

    private:
        ContentItem m_contentItem;
    };

    Page *appendPage(Page *page);

    LinkedList<Page, &Page::m_stackNode> m_pages;
};

}}

#endif
