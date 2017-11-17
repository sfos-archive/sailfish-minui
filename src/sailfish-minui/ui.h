/****************************************************************************
**
** Copyright (C) 2017 Jolla Ltd.
** Contact: Andrew den Exter <andrew.den.exter@jolla.com>
**
****************************************************************************/

#ifndef SAILFISH_MINUI_UI_H
#define SAILFISH_MINUI_UI_H

#include <stdint.h>
#include <minui/minui.h>

#include <functional>
#include <vector>

#define qtTrId(string) string

namespace Sailfish { namespace MinUi {

struct Color {
    Color() = default;
    constexpr Color(const Color &color) = default;
    constexpr Color(char r, char g, char b, char a) : r(r), g(g), b(b), a(a) {}

    char r = 255, g = 255, b = 255, a = 255;
};

struct Palette
{
    Color normal { 240, 240, 240, 255 };
    Color disabled { 20, 20, 20, 255 };
    Color selected { 255, 255, 255, 255 };
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

double pixelRatio();
int scale(int value);

class Geometry
{
public:
    Geometry();
    virtual ~Geometry();

    int x() const { return m_x; }
    int y() const { return m_y; }

    int width() const { return m_width; }
    int height() const { return m_height; }

    int left() const { return m_x; }
    int top() const { return m_y; }
    int right() const { return m_x + m_width; }
    int bottom() const { return m_y + m_height; }

    bool contains(int x, int y, int margin = 0) const;

protected:
    virtual void invalidate() = 0;

    void setX(int x);
    void setY(int y);
    void move(int x, int y);

    void align(HorizontalGuide guide, const Geometry &geometry, HorizontalGuide geometryGuide, int margin = 0);
    void align(VerticalGuide guide, const Geometry &geometry, VerticalGuide geometryGuide, int margin = 0);
    void centerIn(const Geometry &geometry);
    void centerBetween(
            const Geometry &first, HorizontalGuide firstGuide, const Geometry &second, HorizontalGuide secondGuide);
    void centerBetween(
            const Geometry &first, VerticalGuide firstGuide, const Geometry &second, VerticalGuide secondGuide);

    void setWidth(int width);
    void setHeight(int height);
    void resize(int width, int height);
    void fill(const Geometry &geometry, int margin = 0);
    void horizontalFill(const Geometry &geometry, int margin = 0);
    void verticalFill(const Geometry &geometry, int margin = 0);

private:
    inline int position(HorizontalGuide guide) const;
    inline int position(VerticalGuide guide) const;
    inline void setPosition(HorizontalGuide guide, int position);
    inline void setPosition(VerticalGuide guide, int position);

    int m_x = 0;
    int m_y = 0;
    int m_width = 0;
    int m_height = 0;
};

class EventLoop;
class Window;
class Item : public Geometry
{
public:
    explicit Item(Window *window = nullptr);
    ~Item();

    bool isInteractive() const { return m_interactive; }
    bool isPressed() const { return m_pressed; }
    bool isSelected() const { return m_selected; }
    bool isVisible() const { return m_visible; }
    void setVisible(bool visible);

    using Geometry::setX;
    using Geometry::setY;
    using Geometry::move;
    using Geometry::align;
    using Geometry::centerIn;
    using Geometry::centerBetween;

    Window *window() const { return m_window; }
    EventLoop *eventLoop() const;

protected:
    friend class Window;

    void setInteractive(bool interactive);
    void setPressed(bool pressed);
    void setSelected(bool selected);

    virtual void draw() = 0;
    void invalidate() override;

    virtual void activate();
    virtual void stateChanged();

    Window *m_window;
    bool m_interactive = false;
    bool m_pressed = false;
    bool m_selected = false;
    bool m_visible = true;
};

class Window : public Geometry
{
public:
    explicit Window(EventLoop *eventLoop, const char *locale);
    ~Window();

    EventLoop *eventLoop() const { return m_eventLoop; }

    const char *locale() const { return m_locale; }

    Item *selectedItem() const { return m_selectedItem; }
    void setSelectedItem(Item *item);

    Color color() const { return m_color; }
    void setColor(Color color);

    int opacity() const { return m_opacity; }
    void setOpacity(int opacity);

    void draw();

protected:
    void invalidate() override;

private:
    friend class EventLoop;
    friend class Item;

    inline void appendItem(Item *item);
    inline void removeItem(Item *item);
    inline void setInteractive(Item *item, bool interactive);

    inline void updateInteractiveItems();

    inline Item *previous(Item *item) const;
    inline Item *next(Item *item) const;

    void inputEvent(const input_event &event);

    static inline int draw_callback(int fd, uint32_t epevents, void *data);

    struct {
        Item *item = nullptr;
        int id = -1;
        int x = 0;
        int y = 0;
        bool active = false;
        bool pressed = false;
        bool moved = false;
        bool released = false;
    } m_touch;
    EventLoop * const m_eventLoop;
    const char * const m_locale;
    std::vector<Item *> m_items;
    std::vector<Item *> m_interactiveItems;
    Item *m_selectedItem = nullptr;
    Item *m_pressedItem = nullptr;
    const int m_eventFd;
    int m_opacity = 255;
    Color m_color { 0, 0, 0, 255 };
    bool m_interactiveItemsInvalidated = false;
};

class Image : public Item
{
public:
    explicit Image(const char *name, Window *window);
    ~Image();

protected:
    void draw() override;

private:
    gr_surface m_image;
};

class Label : public Item
{
public:
    explicit Label(const char *name, Window *window);
    ~Label();

    Color color() const { return m_color; }
    void setColor(Color color);

protected:
    void draw() override;

private:
    gr_surface m_text;
    Color m_color;
};

class Rectangle : public Item
{
public:
    explicit Rectangle(Window *window);
    ~Rectangle();

    Color color() const { return m_color; }
    void setColor(Color color);

    using Geometry::resize;
    using Geometry::setWidth;
    using Geometry::setHeight;
    using Geometry::horizontalFill;
    using Geometry::verticalFill;
    using Geometry::fill;

protected:
    void draw() override;

private:
    Color m_color;
};

class Button : public Label
{
public:
    explicit Button(const char *name, Window *window);
    ~Button();

    Palette palette() const { return m_palette; }
    void setPalette(const Palette &palette);

    void onActivated(const std::function<void()> &callback);

    using Item::setSelected;
    using Item::setInteractive;

protected:
    void activate() override;
    void stateChanged() override;

private:
    std::function<void()> m_activated;
    Palette m_palette;
};

}}

#endif
