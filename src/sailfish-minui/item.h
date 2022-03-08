/*
 * Copyright (c) 2017-2019 Jolla Ltd.
 *
 * License: Proprietary
 */

#ifndef SAILFISH_MINUI_ITEM_H
#define SAILFISH_MINUI_ITEM_H

#include <stdint.h>
#include <sailfish-minui/linkedlist.h>

#include <functional>

#include <minui/minui.h>

#define qtTrId(string) string

namespace Sailfish { namespace MinUi {

class MultiTouch;

struct Color {
    Color() = default;
    constexpr Color(const Color &color) = default;
    constexpr Color(uint8_t r, uint8_t g, uint8_t b, uint8_t a) : r(r), g(g), b(b), a(a) {}

    uint8_t r = 255, g = 255, b = 255, a = 255;
};

struct Palette
{
    Color normal { 255, 255, 255, 255 };
    Color disabled { 60, 60, 60, 255 };
    Color selected { 180, 180, 180, 255 };
    Color pressed { 127, 223, 255, 255 };
};

enum HorizontalGuide {
    Left,
    HorizontalCenter,
    Right
};

enum VerticalGuide {
    Top,
    VerticalCenter,
    Bottom
};

enum class HorizontalAlignment
{
    Left,
    Center,
    Right
};

class Theme
{
public:
    enum SizeCategory {
        Small,
        Medium,
        Large,
        ExtraLarge
    };

    Theme();
    explicit Theme(double pixelRatio, SizeCategory category);

    int scale(int value) const;

    const double pixelRatio;
    const SizeCategory sizeCategory;

    const int itemSizeExtraSmall = scale(70);
    const int itemSizeSmall = scale(80);
    const int itemSizeMedium = scale(100);
    const int itemSizeLarge = scale(110);
    const int itemSizeExtraLarge = scale(135);
    const int itemSizeHuge = scale(180);

    const int iconSizeExtraSmall = scale(24);
    const int iconSizeSmall = scale(32);
    const int iconSizeSmallPlus = scale(48);
    const int iconSizeMedium = scale(64);
    const int iconSizeLarge = scale(96);
    const int iconSizeExtraLarge = scale(128);
    const int iconSizeLauncher = scale(86);

    const int buttonWidthSmall = scale(234);
    const int buttonWidthMedium = scale(292);
    const int buttonWidthLarge = scale(444);

    const int paddingSmall = scale(6);
    const int paddingMedium = scale(12);
    const int paddingLarge = scale(24);
    const int horizontalPageMargin = sizeCategory >= Large ? paddingLarge * 2 : paddingLarge;
};

extern const Theme theme;

class EventLoop;
class Window;

class Item
{
public:
    enum ItemFlag {
        NotifyOnInputFocusChanges = 0x01,
        PowerButtonDoesntSelect = 0x02
    };

    explicit Item(Item *parent = nullptr);
    Item(const Item &item) = delete;
    virtual ~Item();

    Item &operator =(const Item &item) = delete;

    int x() const { return m_x; }
    void setX(int x);

    int y() const { return m_y; }
    void setY(int y);

    int width() const { return m_width; }
    int height() const { return m_height; }

    int left() const { return m_x; }
    int top() const { return m_y; }
    int right() const { return m_x + m_width; }
    int bottom() const { return m_y + m_height; }

    void move(int x, int y);
    void align(HorizontalGuide guide, const Item &item, HorizontalGuide itemGuide, int margin = 0);
    void align(VerticalGuide guide, const Item &item, VerticalGuide itemGuide, int margin = 0);
    void centerIn(const Item &item);
    void centerBetween(
            const Item &first, HorizontalGuide firstGuide, const Item &second, HorizontalGuide secondGuide);
    void centerBetween(
            const Item &first, VerticalGuide firstGuide, const Item &second, VerticalGuide secondGuide);

    bool contains(int x, int y, bool relative = false) const;

    double opacity() const { return m_opacity; }
    void setOpacity(double opacity);

    bool isEnabled() const { return m_enabled; }
    void setEnabled(bool enabled);

    bool canActivate() const { return m_canActivate; }

    bool keyFocusOnPress() const { return m_keyFocusOnPress; }
    bool acceptsKeyFocus() const { return m_acceptsKeyFocus; }
    bool hasKeyFocus() const;

    bool inputFocusOnPress() const { return m_inputFocusOnPress; }
    bool acceptsInputFocus() const { return m_acceptsInputFocus; }
    bool hasInputFocus() const;

    bool isPressed() const;

    bool isVisible() const { return m_visible; }
    void setVisible(bool visible);

    bool isAncestorOf(const Item *item) const;

    Item *parent() const { return m_parent; }
    Window *window() const { return m_window; }
    EventLoop *eventLoop() const;
    const char *locale() const;

    enum ComparisonResult {
        Match           = 0x01,
        SkipChildren    = 0x2
    };

    enum SearchOptions {
        Wrap            = 0x01,
        AcceptParent    = 0x02
    };

    typedef std::function<int(const Item *item)> ComparisonFunction;

    Item *findItemAt(int x, int y, const ComparisonFunction &comparison);

    Item *findNextChild(const ComparisonFunction &comparison);
    Item *findPreviousChild(const ComparisonFunction &comparison);

    Item *findNextItem(const ComparisonFunction &comparison,  int options = 0);
    Item *findPreviousItem(const ComparisonFunction &comparison, int options = 0);

protected:
    friend class Window;

    enum InvalidateFlag {
        Draw        = 0x01,
        Layout      = 0x02,
        State       = 0x04,
        Enabled     = 0x08,
        InputFocus  = 0x10
    };

    virtual void activate();
    virtual bool keyPress(int code, char character);
    virtual void draw(int dx, int dy, double opacity);
    virtual void updateState(bool enabled);
    virtual void layout();

    void invalidate(int flags);
    void invalidateParent(int flags);

    int itemFlags() const { return m_itemFlags; }
    void setItemFlags(int flags);

    void prependChild(Item *item);
    void appendChild(Item *item);
    void insertChildBefore(Item *sibling, Item *item);
    void insertChildAfter(Item *sibling, Item *item);
    void removeChild(Item *item);

    void setWidth(int width);
    void setHeight(int height);
    void resize(int width, int height);
    void fill(const Item &item, int margin = 0);
    void horizontalFill(const Item &item, int margin = 0);
    void verticalFill(const Item &item, int margin = 0);

    void setCanActivate(bool activate);
    void setKeyFocusOnPress(bool focus);
    void setAcceptsKeyFocus(bool accept);

    void setInputFocusOnPress(bool focus);
    void setAcceptsInputFocus(bool accept);

    inline void invalidateSize();
    inline void invalidateFocus();

private:
    inline int position(HorizontalGuide guide, const Item *relativeTo) const;
    inline int position(VerticalGuide guide, const Item *relativeTo) const;
    inline void setPosition(HorizontalGuide guide, int position);
    inline void setPosition(VerticalGuide guide, int position);

    inline void layoutItems();
    inline void updateItems(int windowFlags, bool enabled);
    inline void drawItems(int dx, int dy, double opacity);
    inline void updateParent(Item *parent);
    inline void updateWindow(Window *window);

    std::string m_objectName;

    Item *m_parent = nullptr;
    Window *m_window = nullptr;
    LinkedListNode m_childrenNode;
    LinkedList<Item, &Item::m_childrenNode> m_children;
    double m_opacity = 1.;
    int m_x = 0;
    int m_y = 0;
    int m_width = 0;
    int m_height = 0;
    int m_itemFlags = 0;
    int m_invalidatedFlags = 0;
    bool m_enabled = true;
    bool m_canActivate = false;
    bool m_keyFocusOnPress = false;
    bool m_acceptsKeyFocus = false;
    bool m_inputFocusOnPress = false;
    bool m_acceptsInputFocus = false;
    bool m_visible = true;

protected:
    typedef LinkedList<Item, &Item::m_childrenNode> ChildList;
    ChildList &childItems();
};

class Window : public Item
{
public:
    enum HapticEffect {
        KeyPressEffect,
        HapticCount
    };
    explicit Window(EventLoop *eventLoop);
    ~Window();

    Item *keyFocusItem() const { return m_keyFocusItem; }
    void setKeyFocusItem(Item *item);

    Item *inputFocusItem() const { return m_inputFocusItem; }
    void setInputFocusItem(Item *item);

    Color color() const { return m_color; }
    void setColor(Color color);

    using Item::prependChild;
    using Item::appendChild;
    using Item::insertChildBefore;
    using Item::insertChildAfter;
    using Item::removeChild;

    bool keyPress(int code, char character) override;

    void playHaptic(HapticEffect effect);

    void disablePowerButtonSelect();

protected:
    void draw(int x, int y, double opacity) override;

private:
    friend class EventLoop;
    friend class Item;

    using Item::setX;
    using Item::setY;
    using Item::move;
    using Item::align;
    using Item::centerIn;
    using Item::centerBetween;

    inline void clearKeyFocus(Item *item);
    inline void clearInputFocus(Item *item);
    inline void clearFocus(Item *item);

    inline Item *previousKeyFocusItem(Item *item, int options = 0);
    inline Item *nextKeyFocusItem(Item *item, int options = 0);

    void inputEvent(int fd, const input_event &event);

    static inline int update_callback(int fd, uint32_t epevents, void *data);

    void fingerPressed(int x, int y);
    void fingerMoved(int x, int y);
    void fingerLifted(int x, int y);

    static void fingerPressed(int x, int y, void *callbackData);
    static void fingerMoved(int x, int y, void *callbackData);
    static void fingerLifted(int x, int y, void *callbackData);

    struct Axis {
        int value = 0;
        int delta = 0;
        int numerator = 1;
        int denomintator = 1;
        int offset = 0;
        bool changed = false;
        bool initialized = false;

        int scale(int value) const { return ((value - offset) * numerator) / denomintator; }
        void initialize(int fd, int code, int screenSize);
    };
    struct {
        Item *item = nullptr;
        Axis x;
        Axis y;
    } m_touch;
    Item *m_keyFocusItem = nullptr;
    Item *m_inputFocusItem = nullptr;
    Item *m_pressedItem = nullptr;
    const int m_eventFd;
    int m_effectFd = -1;
    int m_effectIds[HapticCount] = { -1 };
    Color m_color { 0, 0, 0, 255 };
    MultiTouch *m_multiTouch;
};

class ResizeableItem : public Item
{
public:
    explicit ResizeableItem(Item *parent = nullptr);
    ~ResizeableItem();

    using Item::resize;
    using Item::setWidth;
    using Item::setHeight;
    using Item::horizontalFill;
    using Item::verticalFill;
    using Item::fill;
};

class ContainerItem : public ResizeableItem
{
public:
    explicit ContainerItem(Item *parent = nullptr);
    ~ContainerItem();

//    using Item::children;
    using Item::prependChild;
    using Item::appendChild;
    using Item::insertChildBefore;
    using Item::insertChildAfter;
    using Item::removeChild;
};

class ActivatableItem : public ResizeableItem
{
public:
    explicit ActivatableItem(Item *parent = nullptr);
    ~ActivatableItem();

    void onActivated(const std::function<void()> &callback);

    using Item::setAcceptsKeyFocus;
    using Item::setKeyFocusOnPress;
    using Item::setCanActivate;

    void setSelected(bool selected) { if (window() && selected) { window()->setKeyFocusItem(this); } }

protected:
    void activate() override;

private:
    std::function<void()> m_activated;
};

}}

#endif
