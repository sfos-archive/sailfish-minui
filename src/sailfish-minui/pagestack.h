/****************************************************************************
**
** Copyright (C) 2017 Jolla Ltd.
** Contact: Andrew den Exter <andrew.den.exter@jolla.com>
**
****************************************************************************/

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
