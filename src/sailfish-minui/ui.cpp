/****************************************************************************
**
** Copyright (C) 2017 Jolla Ltd.
** Contact: Andrew den Exter <andrew.den.exter@jolla.com>
**
****************************************************************************/

#include "ui.h"
#include "eventloop.h"

#include <algorithm>
#include <linux/input.h>
#include <sys/eventfd.h>
#include <unistd.h>
#include <string.h>

#include <cassert>
#include <iostream>

extern "C" {
char res_path[128] = "/usr/share/themes/sailfish-default/meegotouch/icons";
}

namespace Sailfish { namespace MinUi {

double pixelRatio()
{
    static double pixelRatio = []() {
        if (const char * const env = getenv("SAILFISH_PIXEL_RATIO")) {
            const double ratio = atof(env);
            if (ratio != 0.) {
                return ratio;
            }
        }
        return 1.;
    }();
    return pixelRatio;
}

int scale(int value)
{
    return std::round(value * pixelRatio());
}

Geometry::Geometry()
{
}

Geometry::~Geometry()
{
}
void Geometry::setX(int x)
{
    m_x = x;
    invalidate();
}

void Geometry::setY(int y)
{
    m_y = y;
    invalidate();
}

void Geometry::move(int x, int y)
{
    m_x = x;
    m_y = y;
    invalidate();
}

bool Geometry::contains(int x, int y, int margin) const
{
    return x >= m_x - margin  && x < m_x + m_width + margin
            && y >= m_y - margin && y < m_y + m_height + margin;
}

void Geometry::align(HorizontalGuide guide, const Geometry &geometry, HorizontalGuide geometryGuide, int margin)
{
    setPosition(guide, geometry.position(geometryGuide) + margin);
}

void Geometry::align(VerticalGuide guide, const Geometry &geometry, VerticalGuide geometryGuide, int margin)
{
    setPosition(guide, geometry.position(geometryGuide) + margin);
}

void Geometry::centerIn(const Geometry &geometry)
{
    centerBetween(geometry, HorizontalCenter, geometry, HorizontalCenter);
    centerBetween(geometry, VerticalCenter, geometry, VerticalCenter);
}

void Geometry::centerBetween(
        const Geometry &first, HorizontalGuide firstGuide, const Geometry &second, HorizontalGuide secondGuide)
{
    const int firstX = first.position(firstGuide);
    const int secondX = second.position(secondGuide);

    setX(firstX + ((secondX - firstX - m_width) / 2));
}

void Geometry::centerBetween(
        const Geometry &first, VerticalGuide firstGuide, const Geometry &second, VerticalGuide secondGuide)
{
    const int firstY = first.position(firstGuide);
    const int secondY = second.position(secondGuide);

    setY(firstY + ((secondY - firstY - m_height) / 2));
}

void Geometry::setWidth(int width)
{
    m_width = width;
    invalidate();
}

void Geometry::setHeight(int height)
{
    m_height = height;
    invalidate();
}

void Geometry::resize(int width, int height)
{
    m_width = width;
    m_height = height;
    invalidate();
}

void Geometry::fill(const Geometry &geometry, int margin)
{
    horizontalFill(geometry, margin);
    verticalFill(geometry, margin);
}

void Geometry::horizontalFill(const Geometry &geometry, int margin)
{
    align(Left, geometry, Left, margin);
    setWidth(geometry.width() - (2 * margin));
}

void Geometry::verticalFill(const Geometry &geometry, int margin)
{
    align(Top, geometry, Top, margin);
    setHeight(geometry.height() - (2 * margin));
}

int Geometry::position(HorizontalGuide guide) const
{
    switch (guide) {
    case Left:
        return left();
    case HorizontalCenter:
        return m_x + (m_width / 2);
    case Right:
        return right();
    }
    return 0;
}

int Geometry::position(VerticalGuide guide) const
{
    switch (guide) {
    case Top:
        return top();
    case VerticalCenter:
        return m_y + (m_height / 2);
    case Bottom:
        return bottom();
    }
    return 0;
}

void Geometry::setPosition(HorizontalGuide guide, int position)
{
    switch (guide) {
    case Left:
        setX(position);
        break;
    case HorizontalCenter:
        setX(position - (m_width / 2));
        break;
    case Right:
        setX(position - m_width);
        break;
    }
}

void Geometry::setPosition(VerticalGuide guide, int position)
{
    switch (guide) {
    case Top:
        setY(position);
        break;
    case VerticalCenter:
        setY(position - (m_height / 2));
        break;
    case Bottom:
        setY(position - m_height);
        break;
    }
}

Item::Item(Window *window)
    : m_window(window)
{
    if (m_window) {
        m_window->appendItem(this);
    }
}

Item::~Item()
{
    if (m_window) {
        m_window->removeItem(this);
    }
}

void Item::setInteractive(bool interactive)
{
    m_interactive = interactive;
    if (m_window) {
        m_window->setInteractive(this, interactive);
    }
    stateChanged();
}

void Item::setPressed(bool pressed)
{
    m_pressed = pressed;
    stateChanged();
}

void Item::setSelected(bool selected)
{
    m_selected = selected;
    stateChanged();
}

void Item::setVisible(bool visible)
{
    m_visible = visible;
    stateChanged();
    invalidate();
}

EventLoop *Item::eventLoop() const
{
    return m_window->eventLoop();
}

void Item::invalidate()
{
    if (m_window) {
        m_window->invalidate();
    }
}

void Item::activate()
{
}

void Item::stateChanged()
{
}

Window::Window(EventLoop *eventLoop, const char *locale)
    : m_eventLoop(eventLoop)
    , m_locale(locale)
    , m_eventFd(::eventfd(0, EFD_NONBLOCK))
{
    m_eventLoop->m_window = this;

    if (const char * const env = getenv("SAILFISH_ICON_DIR")) {
        const size_t length = strlen(env);
        if (length > 0 && length < sizeof(res_path) - 1) {
            strcpy(res_path, env);
        }
    }

    if (gr_init() < 0) {
        std::cerr << "Failed to initialize MinUI graphics" << std::endl;
        ::exit(EXIT_FAILURE);
    }

    resize(gr_fb_width(), gr_fb_height());
    if (m_eventFd >= 0) {
        ev_add_fd(m_eventFd, draw_callback, this);
    }
}

Window::~Window()
{
    m_eventLoop->m_window = nullptr;

    if (m_eventFd >= 0) {
        ::close(m_eventFd);
    }
    gr_exit();
}

void Window::setSelectedItem(Item *item)
{
    if (m_selectedItem) {
        m_selectedItem->setSelected(false);
    }
    m_selectedItem = item;
    if (m_selectedItem) {
        m_selectedItem->setSelected(true);
    }
}

void Window::setColor(Color color)
{
    m_color = color;
    invalidate();
}

void Window::setOpacity(int opacity)
{
    opacity = std::min(255, std::max(0, opacity));
    if (m_opacity != opacity) {
        m_opacity = opacity;
        invalidate();
    }
}

void Window::draw()
{
    gr_color(m_color.r, m_color.g, m_color.b, m_color.a);
    gr_clear();
    for (Item *item : m_items) {
        if (item->isVisible()) {
            item->draw();
        }
    }
    gr_flip();
}

void Window::appendItem(Item *item)
{
    m_items.push_back(item);
    invalidate();
}

void Window::removeItem(Item *item)
{
    if (m_pressedItem == item) {
        m_pressedItem = nullptr;
    }
    if (m_selectedItem == item) {
        Item * const replacement = m_interactiveItems.back() == item
                ? previous(item)
                : next(item);

        if (m_selectedItem != replacement) {
            m_selectedItem = replacement;
            m_selectedItem->setSelected(true);
        } else {
            m_selectedItem = nullptr;
        }
    }

    m_interactiveItems.erase(
                std::remove(m_interactiveItems.begin(), m_interactiveItems.end(), item),
                m_interactiveItems.end());
    m_items.erase(std::remove(m_items.begin(), m_items.end(), item), m_items.end());

    invalidate();
}

void Window::setInteractive(Item *item, bool interactive)
{
    if (interactive) {
        m_interactiveItemsInvalidated = true;
    } else {
        if (m_pressedItem == item) {
            m_pressedItem->setPressed(false);
            m_pressedItem = nullptr;
        }
        if (m_selectedItem == item) {
            m_selectedItem->setSelected(false);

            Item * const replacement = m_interactiveItems.back() == item
                    ? previous(item)
                    : next(item);

            if (m_selectedItem != replacement) {
                m_selectedItem = replacement;
                m_selectedItem->setSelected(true);
            } else {
                m_selectedItem = nullptr;
            }
        }
        m_interactiveItems.erase(
                    std::remove(m_interactiveItems.begin(), m_interactiveItems.end(), item),
                    m_interactiveItems.end());
    }
}

void Window::invalidate()
{
    if (m_eventFd >= 0) {
        int64_t eventData = 1;
        const ssize_t size = ::write(m_eventFd, &eventData, sizeof(eventData));
        assert(size == sizeof(eventData));
    }
}

Item *Window::previous(Item *item) const
{
    auto it = std::find(m_interactiveItems.begin(), m_interactiveItems.end(), item);
    if (it == m_interactiveItems.begin()) {
        it = m_interactiveItems.end();
    }
    --it;

    return *it;
}

Item *Window::next(Item *item) const
{
    auto it = std::find(m_interactiveItems.begin(), m_interactiveItems.end(), item);
    ++it;
    if (it == m_interactiveItems.end()) {
        it = m_interactiveItems.begin();
    }

    return *it;
}

void Window::inputEvent(const input_event &event)
{
    if (m_interactiveItemsInvalidated) {
        m_interactiveItems.clear();
        for (Item *item : m_items) {
            if (item->isInteractive()) {
                m_interactiveItems.push_back(item);
            }
        }
    }

    if (event.type == EV_KEY) {
        switch (event.code) {
        case KEY_DOWN:
        case KEY_VOLUMEDOWN:
        case KEY_UP:
        case KEY_VOLUMEUP:
            if (m_selectedItem && event.value) {
                Item * const item = event.code == KEY_DOWN || event.code == KEY_VOLUMEDOWN
                        ? previous(m_selectedItem)
                        : next(m_selectedItem);
                if (m_selectedItem != item) {
                    m_selectedItem->setSelected(false);
                    m_selectedItem = item;
                    m_selectedItem->setSelected(true);
                }
            }
            break;
        case KEY_ENTER:
        case KEY_POWER:
            if (Item * const item = m_selectedItem) {
                if (event.value) {
                    item->setPressed(true);
                } else if (item->isPressed()) {
                    item->activate();
                    item->setPressed(false);
                }
            }
            break;
        default:
            break;
        }
    } else if (event.type == EV_ABS) {
        switch (event.code) {
        case ABS_MT_TRACKING_ID:
            if (m_touch.id == -1) {
                m_touch.id = event.value;
                m_touch.active = true;
                m_touch.pressed = true;
            } else if (m_touch.active && event.value == -1) {
                m_touch.id = -1;
                m_touch.active = false;
                m_touch.released = true;
            } else {
                m_touch.active = false;
            }
            break;
        case ABS_MT_SLOT:
            m_touch.active = m_touch.id == event.value;
            break;
        case ABS_MT_POSITION_X:
            if (m_touch.active) {
                m_touch.x = event.value;
                m_touch.moved = true;
            }
            break;
        case ABS_MT_POSITION_Y:
            if (m_touch.active) {
                m_touch.y = event.value;
                m_touch.moved = true;
            }
            break;
        }
    } else if (event.type == 0) {
        if (m_touch.pressed) {
            m_touch.pressed = false;
            m_touch.moved = false;

            for (Item *item : m_interactiveItems) {
                if (item->contains(m_touch.x, m_touch.y)) {
                    m_touch.item = item;
                    break;
                }
            }
            if (!m_touch.item) {
                // If no item is an exact match try again with a larger touch area.
                for (Item *item : m_interactiveItems) {
                    if (item->contains(m_touch.x, m_touch.y, 20)) {
                        m_touch.item = item;
                        break;
                    }
                }
            }
            if (m_touch.item) {
                m_touch.item->setSelected(true);
                m_touch.item->setPressed(true);
            } else {
                m_touch.id = -1;
                m_touch.active = false;
                m_touch.released = false;
            }
        }
        if (m_touch.released) {
            m_touch.released = false;
            m_touch.moved = false;

            if (m_touch.item) {
                if (m_touch.item->contains(m_touch.x, m_touch.y, 20)) {
                    m_touch.item->activate();
                }
                m_touch.item->setPressed(false);
                m_touch.item = nullptr;
            }
        }
        if (m_touch.moved) {
            m_touch.moved = false;

            if (m_touch.item && !m_touch.item->contains(m_touch.x, m_touch.y, 20)) {
                m_touch.id = -1;
                m_touch.item->setPressed(false);
                m_touch.item = nullptr;
            }
        }
    }
}

int Window::draw_callback(int, uint32_t, void *data)
{
    Window * const window = static_cast<Window *>(data);

    int64_t eventData;
    const ssize_t size = ::read(window->m_eventFd, &eventData, sizeof(eventData));
    assert(size == sizeof(eventData));

    window->draw();

    return 0;
}

Image::Image(const char *name, Window *window)
    : Item(window)
{
    const int result = res_create_display_surface(name, &m_image);
    if (result != 0) {
        std::cerr << "Failed to load image " << name << " " << result << std::endl;
    }

    resize(gr_get_width(m_image), gr_get_height(m_image));
}

Image::~Image()
{
}

void Image::draw()
{
    gr_blit_rgb(m_image, 0, 0, width(), height(), x(), y());
}

Label::Label(const char *name, Window *window)
    : Item(window)
{
    const int result = res_create_localized_alpha_surface(name, window->locale(), &m_text);
    if (result != 0) {
        std::cerr << "Failed to load label " << name << " " << result << std::endl;
    }

    resize(gr_get_width(m_text), gr_get_height(m_text));
}

Label::~Label()
{
}

void Label::setColor(Color color)
{
    m_color = color;
    invalidate();
}

void Label::draw()
{
    const char alpha = (m_color.a * m_window->opacity()) / 255;
    if (alpha != 0) {
        gr_color(m_color.r, m_color.g, m_color.b, alpha);
        gr_texticon(x(), y(), m_text);
    }
}

Rectangle::Rectangle(Window *window)
    : Item(window)
{
}

Rectangle::~Rectangle()
{
}

void Rectangle::setColor(Color color)
{
    m_color = color;
    invalidate();
}

void Rectangle::draw()
{
    const char alpha = (m_color.a * m_window->opacity()) / 255;
    if (alpha != 0) {
        gr_color(m_color.r, m_color.g, m_color.b, alpha);
        gr_fill(left(), top(), right(), bottom());
    }
}

Button::Button(const char *name, Window *window)
    : Label(name, window)
{
}

Button::~Button()
{
}

void Button::setPalette(const Palette &palette)
{
    m_palette = palette;
    invalidate();
}

void Button::onActivated(const std::function<void()> &callback)
{
    m_activated = callback;
}

void Button::activate()
{
    m_activated();
}

void Button::stateChanged()
{
    if (isPressed()) {
        setColor(m_palette.pressed);
    } else if (!isInteractive()) {
        setColor(m_palette.disabled);
    } else if (!isSelected()) {
        setColor(m_palette.selected);
    } else {
        setColor(m_palette.normal);
    }
}

}}
