/*
 * Copyright (C) 2017-2019 Jolla Ltd.
 *
 * License: Proprietary
 */

#include "ui.h"
#include "eventloop.h"
#include "multitouch.h"
#include "logging.h"

#include <minui/minui.h>

#include <linux/input.h>

#include <sys/eventfd.h>
#include <sys/ioctl.h>

#include <unistd.h>
#include <string.h>
#include <limits.h>
#include <fcntl.h>

#include <algorithm>
#include <cassert>

#define BITS_PER_LONG (sizeof(long) * 8)
#define OFF(x)  ((x)%BITS_PER_LONG)
#define BIT(x)  (1UL<<OFF(x))
#define LONG(x) ((x)/BITS_PER_LONG)
#define test_bit(bit, array)    ((array[LONG(bit)] >> OFF(bit)) & 1)

extern "C" {
char res_path[PATH_MAX] = "/usr/share/sailfish-minui/images/default";
}

namespace Sailfish { namespace MinUi {

template <typename T> struct EffectConstants
{
    T type;
    uint16_t duration;
};

class EnvironmentConstants
{
public:
    EnvironmentConstants();

    char locale[6];
    double pixelRatio = 1.;
    Theme::SizeCategory sizeCategory = Theme::Medium;
    EffectConstants<ff_rumble_effect> rumbleEffects[Window::HapticCount] = {
        { { 0, 0x8000 }, 100 } // KeyPress
    };
};

EnvironmentConstants::EnvironmentConstants()
{
    if (const char * const env = getenv("SAILFISH_ICON_DIR")) {
        const size_t length = strlen(env);
        if (length > 0 && length < sizeof(res_path) - 1) {
            strcpy(res_path, env);
        }
    }

    if (const char *env = getenv("LANG")) {
        strncpy(locale, env, 5);
        locale[5] = '\0';
    } else {
        strncpy(locale, "en_US", 6);
    }

    char resourceLink[PATH_MAX];
    const char *resourcePath = res_path;

    // Resolve the images directory name.
    ssize_t length = readlink(res_path, resourceLink, PATH_MAX - 2);
    if (length > 0) {
        resourceLink[length] = '\0';
        resourcePath = resourceLink;
    }
    for (int i = 0; resourcePath[i]; ++i) {
        if (resourcePath[i] == '/') {
            resourcePath += i + 1;
            i = -1;
        }
    }

    // Extract the pixel ratio and size category from the resolved resources directory
    // which should be of z[ratio](-[category]). For example z1.25 or z1.5-large.
    char categoryBuffer[16] = ""; // largest required is extraLarge.
    const char *category = categoryBuffer;
    sscanf(resourcePath, "z%lf-%15s", &pixelRatio, categoryBuffer);

    if (const char * const env = getenv("SAILFISH_PIXEL_RATIO")) {
        const double ratio = atof(env);
        if (ratio != 0.) {
            pixelRatio = ratio;
        }
    }

    if (const char * const env = getenv("SAILFISH_SIZE_CATEGORY")) {
        category = env;
    }

    if (strcmp(category, "small") == 0) {
        sizeCategory = Theme::Small;
    } else if (strcmp(category, "medium") == 0) {
        sizeCategory = Theme::Medium;
    } else if (strcmp(category, "large") == 0) {
        sizeCategory = Theme::Large;
    } else if (strcmp(category, "extraLarge") == 0) {
        sizeCategory = Theme::ExtraLarge;
    }
}

const EnvironmentConstants &environment()
{
    static const EnvironmentConstants environment;
    return environment;
}

/*!
    \namespace Sailfish
*/

/*!
    \namespace Sailfish::MinUi
*/

/*!
    \class Sailfish::MinUi::Color
    \brief An RGBA color.
*/

/*!
    \class Sailfish::MinUi::Palette
    \brief A color palette
*/

/*!
    \variable Sailfish::MinUi::Palette::normal

    The foreground color of an item in its normal state.
*/

/*!
    \variable Sailfish::MinUi::Palette::disabled

    The foreground color of an item in its disabled state.
*/

/*!
    \variable Sailfish::MinUi::Palette::selected

    The foreground color of an item in its selected state.
*/

/*!
    \variable Sailfish::MinUi::Palette::pressed

    The foreground color of an item in its pressed state.
*/

/*!
    \enum Sailfish::MinUi::HorizontalGuide

    An identifier for a horizontal anchor of an item.

    \value Left The left edge
    \value HorizontalCenter The horizontal center
    \value Right The right edge
*/

/*!
    \enum Sailfish::MinUi::VerticalGuide

    An identifier for a vertical anchor of an item.

    \value Top The top edge
    \value VerticalCenter The vertical center
    \value Top The bottom edge
*/

/*!
    \class Sailfish::MinUi::Theme
    \brief Sailfish theme constants.
*/

/*!
    \enum Sailfish::MinUi::Theme::SizeCategory

    The screen size profile of a device.

    \value Small A small screen device.
    \value Medium A medium screen device.
    \value Large A large screen device.
    \value ExtraLarge An extra large screen device.
*/

/*!
    Constructs a theme.
*/
Theme::Theme()
    : Theme(environment().pixelRatio, environment().sizeCategory)
{
}

/*!
    Constructs a theme overriding the default \a pixelRatio and size \a category.
*/
Theme::Theme(double pixelRatio, SizeCategory category)
    : pixelRatio(pixelRatio)
    , sizeCategory(category)
{
}

/*!
    Scales a size \a value in reference pixels by the screen pixel ratio.

    Returns the scaled size.
*/
int Theme::scale(int value) const
{
    return std::round(value * pixelRatio);
}

/*!
    \variable Sailfish::MinUi::Theme::pixelRatio

    The ratio between the screen pixel size and a unit theme pixel.
*/

/*!
    \variable Sailfish::MinUi::Theme::sizeCategory;

    The screen size profile of a device.
*/

/*!
    \variable Sailfish::MinUi::Theme::itemSizeExtraSmall

    The size of an extra small item in device pixels.
*/

/*!
    \variable Sailfish::MinUi::Theme::itemSizeSmall

    The size of a small item in device pixels.
*/

/*!
    \variable Sailfish::MinUi::Theme::itemSizeMedium

    The size of a medium item in device pixels.
*/

/*!
    \variable Sailfish::MinUi::Theme::itemSizeLarge

    The size of a large item in device pixels.
*/
/*!
    \variable Sailfish::MinUi::Theme::temSizeExtraLarge

    The size of an extra large item in device pixels.
*/

/*!
    \variable Sailfish::MinUi::Theme::itemSizeHuge

    The size of a huge item in device pixels.
*/

/*!
    \variable Sailfish::MinUi::Theme::buttonWidthSmall

    The width of a small button in device pixels.
*/

/*!
    \variable Sailfish::MinUi::Theme::buttonWidthMedium

    The width of a medium button in device pixels.
*/

/*!
    \variable Sailfish::MinUi::Theme::buttonWidthLarge

    The width of a large button on device pixels.
*/

/*!
    \variable Sailfish::MinUi::Theme::paddingSmall

    The small padding size in device pixels.
*/

/*!
    \variable Sailfish::MinUi::Theme::paddingMedium

    The medium padding size in device pixels.
*/

/*!
    \variable Sailfish::MinUi::Theme::paddingLarge

    The large padding size in device pixels.
*/

/*!
    \variable Sailfish::MinUi::Theme::horizontalPageMargin

    The horizontal page margin in device pixels.
*/

const Theme theme;

/*!
    \class Sailfish::MinUi::Item
    \brief The base class for all graphical or interactive types in a UI scene
*/

/*!
    Constructs a new item.

    If a \a parent argument is supplied the new item will be appended as a child of that item.
*/
Item::Item(Item *parent)
{
    if (parent) {
        parent->appendChild(this);
    }
}

/*!
    Destroys an item.
*/
Item::~Item()
{
    if (m_parent) {
        m_parent->removeChild(this);
    }
}

/*!
    Returns true if the item is currently pressed either by touching it on the screen or by pressing
    the power button while the item has key focus.
*/
bool Item::isPressed() const
{
    return m_window && m_window->m_pressedItem == this;
}

/*!
    Returns true if the item currently has key focus.

    An item with key focus will have its activate() function called if the power key is pressed.
*/
bool Item::hasKeyFocus() const
{
    return m_window && m_window->m_keyFocusItem == this;
}

/*!
    Returns true if the item has input focus.

    An item with input focus will have input key press events delivered to its keyPress() function.

    These events may originate from a hardware keyboard or an on-screen input panel by way of
    the Window::keyPress() function.
*/
bool Item::hasInputFocus() const
{
    return m_window && m_window->m_inputFocusItem == this;
}

/*!
    \fn Sailfish::MinUi::Item::isEnabled()

    Returns true if an item is enabled.

    An enabled item is able to receive touch and key events. Disabling an item will also prevent
    events being delivered to child items.
*/

/*!
    Sets the \a enabled state of an item.
*/
void Item::setEnabled(bool enabled)
{
    if (m_enabled != enabled) {
        m_enabled = enabled;

        if (m_window && !m_enabled) {
            m_window->clearFocus(this);
        }

        invalidate(Enabled | Draw);
    }
}

/*!
    \fn Sailfish::MinUi::Item::canActivate

    Returns true if an item \a can be activated.

    An item which can be activated will have its activate() function called if it is tapped
    on screen or the power button is pressed while it has key focus.
*/
void Item::setCanActivate(bool can)
{
    if (m_canActivate != can) {
        m_canActivate = can;
        if (m_window && m_window->m_pressedItem == this) {
            m_window->m_pressedItem = nullptr;
        }
        invalidate(State);
    }
}

/*!
    \fn Sailfish::MinUi::Item::keyFocusOnPress()

    Returns true if an item will receive key focus when pressed.

    If this property is set tapping the screen within the bounds of the item will give it
    key focus.  If the item itself doesn't accept key focus its first descendent item which
    does will receive key focus instead.
*/

/*!
    Sets whether an item will receive key \a focus when it is pressed.
*/
void Item::setKeyFocusOnPress(bool focus)
{
    m_keyFocusOnPress = focus;
}

/*!
    \fn Sailfish::MinUi::Item::acceptsKeyFocus()

    Returns true if an item can be given key focus.

    An item with key focus will have its activate() function called if the power key is pressed.
*/

/*!
    Sets whether an item will \a accept key focus.
*/
void Item::setAcceptsKeyFocus(bool accept)
{
    if (m_acceptsKeyFocus != accept) {
        m_acceptsKeyFocus = accept;
        if (m_window && !m_acceptsKeyFocus) {
            m_window->clearKeyFocus(this);
        }
    }
}

/*!
    \fn Sailfish::MinUi::Item::inputFocusOnPress()

    Returns true if an item will receive input focus when pressed.

    If this property is set tapping the screen within the bounds of the item will give it
    input focus.  If the item itself doesn't accept input focus its first descendent item which
    does will receive input focus instead.
*/

/*!
    Sets whether an item will receive input \a focus when it is pressed.
*/
void Item::setInputFocusOnPress(bool focus)
{
    m_inputFocusOnPress = focus;
}

/*!
    \fn Sailfish::MinUi::Item::acceptsInputFocus()

    Returns true if an item can be given input focus.

    An item with input focus will have input key press events delivered to its keyPress() function.
*/

/*!
    Sets whether an item will \a accept input focus.
*/
void Item::setAcceptsInputFocus(bool accept)
{
    if (m_acceptsInputFocus != accept) {
        m_acceptsInputFocus = accept;
        if (m_window && m_acceptsInputFocus) {
            m_window->clearInputFocus(this);
        }
    }
}

/*!
    \fn Sailfish::MinUi::Item::isVisible()

    Returns true if an item is visible.

    An visible item is able to be drawn and receive touch and key events. Hiding an item will also
    prevent drawing and events being delivered to child items.
*/

/*!
    Sets the \a visible state of an item.
*/
void Item::setVisible(bool visible)
{
    if (m_visible != visible) {
        m_visible = visible;

        if (m_window && !m_visible) {
            m_window->clearFocus(this);
        }

        invalidate(Draw);

        if (m_parent) {
            m_parent->invalidate(Layout);
        }
    }
}

/*!
    Allows an item to draw itself at the absolute screen coordinates \a x and \a y.

    The coordinates and \a opacity are inclusive of the items own position and opacity.

    A new draw can be triggered by calliing invalidate with the Draw flag.
*/
void Item::draw(int x, int y, double opacity)
{
    (void)x;
    (void)y;
    (void)opacity;
}

/*!
    Allows an item to re-position its child items after it or one of the children were resized.

    A new layout can be triggered by calling invalidate() with the Layout flag.
*/
void Item::layout()
{
}

/*!
    Allows an item to update its internal state following a change to its focus, pressed or
    enabled properties.

    The \a enabled argument will be true if the item is enabled and all of its ancestor items
    are also enabled.
*/
void Item::updateState(bool enabled)
{
    (void)enabled;
}

/*!
    \enum Sailfish::MinUi::Item::InvalidateFlag

    Argumuments to the invalidate() function.

    \value Draw Causes all visible items to be redrawn.
    \value Layout Causes a relayout of an item.
    \value State Causes an update of the state of an item.
    \value Enabled Causes an update of the enabled state of an item and all of its descendent items.
    \value InputFocus Notifies any items with the NotifyOnInputFocusChanges item that the input focus item has changed.
*/

/*!
    Invalidates one or more update \a flags.
*/
void Item::invalidate(int flags)
{
    const int invalidatedFlags = m_invalidatedFlags | flags;
    if (m_invalidatedFlags != invalidatedFlags) {
        m_invalidatedFlags = invalidatedFlags;

        if (m_window) {
            const int windowFlags = m_window->m_invalidatedFlags | flags;
            if ((m_window == this || m_window->m_invalidatedFlags != windowFlags)
                    && m_window->m_eventFd >= 0) {
                m_window->m_invalidatedFlags = windowFlags;

                int64_t eventData = 1;
                const ssize_t size = ::write(m_window->m_eventFd, &eventData, sizeof(eventData));
                assert(size == sizeof(eventData));
            }
        }
    }
}

/*!
    Invalidates one or more update \a flags of an items parent.
*/
void Item::invalidateParent(int flags)
{
    if (m_parent) {
        m_parent->invalidate(flags);
    }
}

/*!
    \enum Sailfish::MinUi::Item::ItemFlags

    \value NotifyOnInputFocusChanges An item with this flag set will have its updateState() function called if the Window input focus item changes.
*/

/*!
    \fn Sailfish::MinUi::Item::itemFlags()

    Returns an item's item flags.
*/

/*!
    Sets an item's item \a flags.
*/
void Item::setItemFlags(int flags)
{
    m_itemFlags = flags;
}

/*!
    \fn Sailfish::MinUi::Item::opacity()

    Returns an items's opacity.
*/

/*!
    Sets an item's opacity.
*/
void Item::setOpacity(double opacity)
{
    opacity = std::min(1., std::max(0., opacity));
    if (m_opacity != opacity) {
        m_opacity = opacity;
        invalidate(Draw);
    }
}

/*!
    Prepends an \a item to a parent's child list.

    If the item was a member of another item's child list it will be removed from that.
*/
void Item::prependChild(Item *item)
{
    item->updateParent(this);

    m_children.prepend(item);
}

/*!
    Appends an \a item to a parent's child list.

    If the item was a member of another item's child list it will be removed from that.
*/
void Item::appendChild(Item *item)
{
    item->updateParent(this);

    m_children.append(item);
}

/*!
    Inserts an \a item to a parent's child list immediately before a \a sibling.

    If the item was a member of another item's child list it will be removed from that.
*/
void Item::insertChildBefore(Item *sibling, Item *item)
{
    item->updateParent(this);

    m_children.insertBefore(sibling, item);
}

/*!
    Inserts an \a item to a parent's child list immediately after a \a sibling.

    If the item was a member of another item's child list it will be removed from that.
*/
void Item::insertChildAfter(Item *sibling, Item *item)
{
    item->updateParent(this);

    m_children.insertAfter(sibling, item);
}

/*!
    Removes an \a item from a parent's child list.
*/
void Item::removeChild(Item *item)
{
    if (item->m_parent == this) {
        item->updateParent(nullptr);

        item->m_childrenNode.erase();
    }
}

/*!
    Returns true if this item is an ancestor of another \a item.
*/
bool Item::isAncestorOf(const Item *item) const
{
    for (; item; item = item->m_parent) {
        if (item == this) {
            return true;
        }
    }
    return false;
}

/*!
    Updates the \a parent of an item.
*/
void Item::updateParent(Item *parent)
{
    if (m_parent == parent) {
        return;
    }

    if (m_window && (!parent || !parent->m_window)) {
        m_window->clearFocus(this);
    }

    m_parent = parent;
    updateWindow(parent ? parent->m_window : nullptr);

    invalidate(Draw | State | Layout);
}

/*!
    Updates the \a window of an item.
*/
void Item::updateWindow(Window *window)
{
    m_window = window;

    for (Item &child : m_children) {
        child.updateWindow(window);
    }
}

/*!
    \Sailfish::MinUi::Item::x() const

    Returns the x coordinate of an item relative to its parent.
*/

/*!
    Sets the \a x coordinate of an item.
*/
void Item::setX(int x)
{
    m_x = x;
    invalidate(Draw);
}

/*!
    \Sailfish::MinUi::Item::y() const

    Returns the x coordinate of an item relative to its parent.
*/

/*!
    Sets the \a y coordinate of an item.
*/
void Item::setY(int y)
{
    m_y = y;
    invalidate(Draw);
}

/*!
    Move the item to the position \a x, \a y.
*/
void Item::move(int x, int y)
{
    m_x = x;
    m_y = y;
    invalidate(Draw);
}

/*!
    Returns true if the point \x, \a y is within the bounds of the item.

    If \a relative is true the position is relative to the items top left otherwise it is in
    absolute screen coordinates.
*/
bool Item::contains(int x, int y, bool relative) const
{
    for (const Item *item = relative ? nullptr : this; item; item = item->m_parent) {
        x -= item->m_x;
        y -= item->m_y;
    }

    return x >= 0 && x < m_width && y >= 0 && y < m_height;
}

/*!
    Positions an item so that the horizontal edge identified by \a guide is aligned to the edge
    of another \a item identified by \a itemGuide separated by \a margin.
*/
void Item::align(HorizontalGuide guide, const Item &item, HorizontalGuide itemGuide, int margin)
{
    setPosition(guide, item.position(itemGuide, this) + margin);
}

/*!
    Positions an item so that the vertical edge identified by \a guide is aligned to the edge
    of another \a item identified by \a itemGuide separated by \a margin.
*/
void Item::align(VerticalGuide guide, const Item &item, VerticalGuide itemGuide, int margin)
{
    setPosition(guide, item.position(itemGuide, this) + margin);
}

/*!
    Positions an item in the center of another \a item.
*/
void Item::centerIn(const Item &item)
{
    centerBetween(item, HorizontalCenter, item, HorizontalCenter);
    centerBetween(item, VerticalCenter, item, VerticalCenter);
}

/*!
    Horizontally centers an item between the edge of a \a first item identified by \a firstGuide
    and the edge of a \a second item identified by \a secondGuide.
*/
void Item::centerBetween(
        const Item &first, HorizontalGuide firstGuide, const Item &second, HorizontalGuide secondGuide)
{
    const int firstX = first.position(firstGuide, this);
    const int secondX = second.position(secondGuide, this);

    setX(firstX + ((secondX - firstX - m_width) / 2));
}

/*!
    Vertically centers an item between the edge of a \a first item identified by \a firstGuide
    and the edge of a \a second item identified by \a secondGuide.
*/
void Item::centerBetween(
        const Item &first, VerticalGuide firstGuide, const Item &second, VerticalGuide secondGuide)
{
    const int firstY = first.position(firstGuide, this);
    const int secondY = second.position(secondGuide, this);

    setY(firstY + ((secondY - firstY - m_height) / 2));
}

/*!
    \fn Sailfish::MinUi::Item::width() const

    Returns the width of an item.
*/

/*!
    Sets the \a width of an item.
*/
void Item::setWidth(int width)
{
    if (m_width != width) {
        m_width = width;
        invalidateSize();
    }
}

/*!
    \fn Sailfish::MinUi::Item::height() const

    Returns the height of an item.
*/

/*!
    Sets the \a height of an item.
*/
void Item::setHeight(int height)
{
    if (m_height != height) {
        m_height = height;
        invalidateSize();
    }
}

/*!
    Sets the \a width and \a height of an item.
*/
void Item::resize(int width, int height)
{
    if (m_width != width || m_height != height) {
        m_width = width;
        m_height = height;
        invalidateSize();
    }
}

/*!
    Invalidates the size of an item.

    This will cause a relayout of the item, and its parent and children.
*/
void Item::invalidateSize()
{
    invalidate(Draw | Layout);
    if (m_parent) {
        m_parent->invalidate(Layout);
    }
    for (Item &child : m_children) {
        child.invalidate(Layout);
    }
}

/*!
    Invalidates the focus of an item.

    This will cause an update of the state of an item and its parent.
*/
void Item::invalidateFocus()
{
    invalidate(Draw | State);
    if (m_parent) {
        m_parent->invalidate(State);
    }
}

/*!
    Resizes an item to fill another \a item with fixed \a margin on all sides.
*/
void Item::fill(const Item &item, int margin)
{
    horizontalFill(item, margin);
    verticalFill(item, margin);
}

/*!
    Resizes an item to horizontally fill another \a item with fixed \a margin on each side.
*/
void Item::horizontalFill(const Item &item, int margin)
{
    align(Left, item, Left, margin);
    setWidth(item.width() - (2 * margin));
}

/*!
    Resizes an item to vertically fill another \a item with fixed \a margin on each side.
*/
void Item::verticalFill(const Item &item, int margin)
{
    align(Top, item, Top, margin);
    setHeight(item.height() - (2 * margin));
}

/*!
    Returns the position of a horizontal \a guide \a relativeTo another item which must be a parent
    or sibling.
*/
int Item::position(HorizontalGuide guide, const Item *relativeTo) const
{
    const int x = this == relativeTo->m_parent ? 0 : m_x;
    switch (guide) {
    case Left:
        return x;
    case HorizontalCenter:
        return x + (m_width / 2);
    case Right:
        return x + m_width;
    }
    return 0;
}

/*!
    Returns the position of a vertical \a guide \a relativeTo another item which must be a parent
    or sibling.
*/
int Item::position(VerticalGuide guide, const Item *relativeTo) const
{
    const int y = this == relativeTo->m_parent ? 0 : m_y;
    switch (guide) {
    case Top:
        return y;
    case VerticalCenter:
        return y + (m_height / 2);
    case Bottom:
        return y + m_height;
    }
    return 0;
}

/*!
    Sets the \a position of a horizontal \a guide.
*/
void Item::setPosition(HorizontalGuide guide, int position)
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

/*!
    Sets the \a position of a vertical \a guide.
*/
void Item::setPosition(VerticalGuide guide, int position)
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

/*!
    Returns the item's event loop.
*/
EventLoop *Item::eventLoop() const
{
    return EventLoop::instance();
}

/*!
    Returns the item's locale.
*/
const char *Item::locale() const
{
    return environment().locale;
}

/*!
    Called when an item is tapped or the power button is pressed while the item has key focus.

    The canActivate() function must return true for this to be called.
*/
void Item::activate()
{
}

/*!
    Called when a key with the given key \a code and \a character is pressed.

    Returns true if the item handles the key press and false otherwise.
*/
bool Item::keyPress(int code, char character)
{
    (void)code;
    (void)character;

    return false;
}

/*!
    Returns a reference to the items list of child items.
*/
Item::ChildList &Item::childItems()
{
    return m_children;
}

/*!
    Finds an item at position \a x, \a y which satisfies a \a comparison function.

    If no such item can be found an null pointer will be returned instead.
*/
Item *Item::findItemAt(int x, int y, const ComparisonFunction &comparison)
{
    const int result = comparison(this);

    if (result != SkipChildren && contains(x, y, true)) {
        if (!(result & SkipChildren)) {
            for (Item &item : m_children) {
                if (Item * const child = item.findItemAt(x - item.m_x, y - item.m_y, comparison)) {
                    return child;
                }
            }
        }

        if (result & Match) {
            return this;
        }
    }
    return nullptr;
}

/*!
    Finds the next child item which satisfies a \a comparison function.

    If no such item can be found an null pointer will be returned instead.
*/
Item *Item::findNextChild(const ComparisonFunction &comparison)
{
    for (Item &item : m_children) {
        const int result = comparison(&item);

        if (Item * const child = !(result & SkipChildren) ? item.findNextChild(comparison) : nullptr) {
            return child;
        } else if (result & Match) {
            return &item;
        }
    }
    return nullptr;
}

/*!
    Finds the previous child item which satisfies a \a comparison function.

    If no such item can be found an null pointer will be returned instead.
*/
Item *Item::findPreviousChild(const ComparisonFunction &comparison)
{
    for (auto it = m_children.end(); it != m_children.begin();) {
        --it;

        Item &item = *it;

        const int result = comparison(&item);

        if (Item * const child = !(result & SkipChildren) ? item.findPreviousChild(comparison) : nullptr) {
            return child;
        } else if (result & Match) {
            return &item;
        }
    }
    return nullptr;
}

/*!
    Finds the next item in the set of all items which satisfies a \a comparison function and \a options.

    If no such item can be found an null pointer will be returned instead.
*/
Item *Item::findNextItem(const ComparisonFunction &comparison, int options)
{
    Item *item = this;
    for (Item *parent; (parent = item->m_parent); item = parent) {
        for (ChildList::iterator it(item->m_childrenNode.next); it != parent->m_children.end(); ++it) {
            Item * const sibling = it;

            const int result = comparison(sibling);

            if (Item * const child = (!result & SkipChildren)
                    ? sibling->findNextChild(comparison)
                    : nullptr) {
                return child;
            } else if (result & Match) {
                return sibling;
            }
        }

        if (options & AcceptParent) {
            const int result = comparison(parent);

            if (result & Match) {
                return parent;
            }
        }
    }

    if ((options & Wrap) && m_window) {
        return m_window->findNextChild([this, &comparison](const Item *item) {
            return item != this ? comparison(item) : SkipChildren;
        });
    } else {
        return nullptr;
    }
}

/*!
    Finds the previous item in the set of all items which satisfies a \a comparison function and \a options.

    If no such item can be found an null pointer will be returned instead.
*/
Item *Item::findPreviousItem(const ComparisonFunction &comparison, int options)
{
    Item *item = this;
    for (Item *parent; (parent = item->m_parent); item = parent) {
        for (ChildList::iterator it(item->m_childrenNode.previous); it != parent->m_children.end(); --it) {
            Item * const sibling = it;

            const int result = comparison(sibling);

            if (Item * const child = (!result & SkipChildren)
                    ? sibling->findPreviousChild(comparison)
                    : nullptr) {
                return child;
            } else if (result & Match) {
                return sibling;
            }
        }
    }

    if ((options & Wrap) && m_window) {
        return m_window->findPreviousChild([this, &comparison](const Item *item) {
            return item != this ? comparison(item) : SkipChildren;
        });
    } else {
        return nullptr;
    }
}

/*!
    Performs a layout of this item and all child items which have the Layout invalidate flag set.
*/
void Item::layoutItems()
{
    if (!m_visible) {
        return;
    }

    if (m_invalidatedFlags & Layout) {
        layout();
        m_invalidatedFlags &= ~Layout;
    }

    for (Item &item : m_children) {
        item.layoutItems();
    }

    if (m_invalidatedFlags & Layout) {
        layout();
        m_invalidatedFlags &= ~Layout;
    }

}

/*!
    Redraws this item and all of its visible child items at a position and opacity relative to
    the accumulated position \a dx, \a dy and \a opacity.
*/
void Item::drawItems(int dx, int dy, double opacity)
{
    if (!m_visible) {
        return;
    }

    dx += m_x;
    dy += m_y;
    opacity *= m_opacity;

    draw(dx, dy, opacity);
    m_invalidatedFlags &= ~Draw;

    for (Item &item : m_children) {
        item.drawItems(dx, dy, opacity);
    }
}

/*!
    Updates the state of this this item and its children with the given \a windowFlags and
    accumulated \a enabled state.
*/
void Item::updateItems(int windowFlags, bool enabled)
{
    // Let State flagged through to pass palette changes to the children
    if (!m_visible && !(m_invalidatedFlags & State)) {
        return;
    }

    enabled = enabled && m_enabled;

    if ((m_invalidatedFlags & (State | Enabled))
            || ((windowFlags & InputFocus) && (m_itemFlags & NotifyOnInputFocusChanges))) {
        updateState(enabled);
        m_invalidatedFlags &= ~(State | Enabled);
    }

    for (Item &item : m_children) {
        item.updateItems(windowFlags, enabled);
    }

    if (m_invalidatedFlags & (State | Enabled)) {
        updateState(enabled);
        m_invalidatedFlags &= ~(State | Enabled);
    }
}

/*!
    \class Sailfish::MinUi::Window
    \brief A fullscreen root scene item
*/

/*!
    Constructs a new window for an \a eventLoop.

    Only one window instance is supported at a time.
*/
Window::Window(EventLoop *eventLoop)
    : Item(nullptr)
    , m_eventFd(::eventfd(0, EFD_NONBLOCK))
    , m_multiTouch(nullptr)
{
    m_window = this;

    eventLoop->m_window = this;

    if (gr_init() < 0) {
        log_err("Failed to initialize MinUI graphics");
        ::exit(EXIT_FAILURE);
    }

    m_multiTouch = new MultiTouch(fingerPressed,
                                  fingerMoved,
                                  fingerLifted,
                                  static_cast<void*>(this));

    resize(gr_fb_width(), gr_fb_height());
    if (m_eventFd >= 0) {
        ev_add_fd(m_eventFd, update_callback, this);
    }

    for (int i = 0; i < 256; ++i) {
        char fileName[24];
        sprintf(fileName, "/dev/input/event%d", i);
        m_effectFd = ::open(fileName, O_RDWR);
        if (m_effectFd == -1) {
            break;
        }

        unsigned long features[4];
        if (ioctl(m_effectFd, EVIOCGBIT(EV_FF, sizeof(features)), features) < 0) {
            ::close(m_effectFd);
            m_effectFd = -1;
            continue;
        }

        if (test_bit(FF_RUMBLE, features)) {
            ff_effect effect;
            memset(&effect, 0, sizeof(effect));
            effect.type = FF_RUMBLE;
            effect.replay.delay = 0;

            for (int j = 0; j < HapticCount; ++j) {
                const auto &constants = environment().rumbleEffects[j];

                effect.id = -1;
                effect.replay.length = constants.duration;
                effect.u.rumble = constants.type;

                if (ioctl(m_effectFd, EVIOCSFF, &effect) != -1) {
                    m_effectIds[j] = effect.id;
                }
            };

            break;
        } else {
            ::close(m_effectFd);
            m_effectFd = -1;
        }
    }

    gr_fb_blank(false);
}

/*!
    Destroys a window.
*/
Window::~Window()
{
    eventLoop()->m_window = nullptr;

    if (m_eventFd >= 0) {
        ::close(m_eventFd);
    }
    if (m_effectFd >= 0) {
        ::close(m_effectFd);
    }

    delete m_multiTouch;
    m_multiTouch = nullptr;

    gr_exit();
}

/*
    Returns true if an \a item can be given focus.

    For an item to receive focus it must be a descendent of \a window and it and all of its ancestor
    items must be both visible and enabled.
*/
static bool itemAllowedFocus(const Window *window, const Item *item)
{
    for (; item; item = item->parent()) {
        if (!item->isEnabled() || !item->isVisible()) {
            return false;
        } else if (item == window) {
            return true;
        }
    }
    return false;
}

/*
    A comparison function which determines if an \a item can receive key focus.
*/
static int isKeyFocusItem(const Item *item)
{
    if (!item->isEnabled() || !item->isVisible()) {
        return Item::SkipChildren;
    }
    return item->acceptsKeyFocus() ? Item::Match : 0;
}

/*!
    \fn Window::keyFocusItem()

    Returns the window's key focus item.

    This is the item that will return power key activation events.
*/

/*!
    Sets the window's key focus \a item.
*/
void Window::setKeyFocusItem(Item *item)
{
    if (m_keyFocusItem) {
        m_keyFocusItem->invalidateFocus();
    }

    if (!itemAllowedFocus(this, item)) {
        m_keyFocusItem = nullptr;
    } else if (item && !item->m_acceptsKeyFocus) {
        m_keyFocusItem = item->findNextChild(isKeyFocusItem);
    } else {
        m_keyFocusItem = item;
    }

    if (m_keyFocusItem) {
        m_keyFocusItem->invalidateFocus();
    }
}

/*
    A comparison function which determines if an \a item can receive input focus.
*/
static int isInputFocusItem(const Item *item)
{
    if (!item->isEnabled() || !item->isVisible()) {
        return Item::SkipChildren;
    }
    return item->acceptsInputFocus() ? Item::Match : 0;
}

/*!
    \fn Window::inputFocusItem()

    Returns the window's input focus item.

    This is the item that will return input key press events.
*/

/*!
    Sets the window's input focus \a item.
*/
void Window::setInputFocusItem(Item *item)
{
    if (m_inputFocusItem != item) {
        if (m_inputFocusItem) {
            m_inputFocusItem->invalidateFocus();
        }

        if (!itemAllowedFocus(this, item)) {
            m_inputFocusItem = nullptr;
        } else if (item && !item->m_acceptsInputFocus) {
            m_inputFocusItem = item->findNextChild(isInputFocusItem);
        } else {
            m_inputFocusItem = item;
        }

        if (m_inputFocusItem) {
            m_inputFocusItem->invalidateFocus();
        }

        invalidate(InputFocus);
    }
}

/*!
    \fn Sailfish::MinUi:::Window::color() const

    Returns the background color of a window.
*/

/*!
    Sets the background \a color of a window.
*/
void Window::setColor(Color color)
{
    m_color = color;
    invalidate(Draw);
}

/*!
    Delivers a key press \a code and \a character to the input focus item.

    Returns true if the event was handled and false otherwise.
*/
bool Window::keyPress(int code, char character)
{
    if (m_inputFocusItem && m_inputFocusItem != this) {
        return m_inputFocusItem->keyPress(code, character);
    }
    return false;
}

/*!
    Plays a simple haptic \a effect.
*/
void Window::playHaptic(HapticEffect effect)
{
    if (effect < HapticCount && m_effectFd != -1 && m_effectIds[effect] != -1) {
        input_event event;
        memset(&event, 0, sizeof(input_event));

        event.type = EV_FF;
        event.code = m_effectIds[effect];
        event.value = 1;

        const ssize_t size = ::write(m_effectFd, &event, sizeof(event));
        (void)size;
    }
}

/*!
    Draws the window background.
*/
void Window::draw(int, int, double)
{
    gr_color(m_color.r, m_color.g, m_color.b, m_color.a);
    gr_clear();
}

/*!
    Removes key focus from an \a item.

    Key focus will be given to the next item that can accept key focus.
*/
void Window::clearKeyFocus(Item *item)
{
    if (item->isAncestorOf(m_keyFocusItem)) {
        Item * const replacement = nextKeyFocusItem(item, Wrap | AcceptParent);

        if (replacement && m_keyFocusItem != replacement) {
            m_keyFocusItem = replacement;
            m_keyFocusItem->invalidateFocus();
        } else {
            m_keyFocusItem = nullptr;
        }
    }
}

/*!
    Removes input focus from an \a item.
*/
void Window::clearInputFocus(Item *item)
{
    if (item->isAncestorOf(m_inputFocusItem)) {
        m_inputFocusItem = nullptr;
        invalidate(InputFocus);
    }
}

/*!
    Clears all focus from an \a item.
*/
void Window::clearFocus(Item *item)
{
    if (item->isAncestorOf(m_pressedItem)) {
        m_pressedItem = nullptr;
    }
    clearKeyFocus(item);
    clearInputFocus(item);
}

/*!
    Returns the first available item prior to \a item that is able to accept key focus and satisfies
    the search \a options.
*/
Item *Window::previousKeyFocusItem(Item *item, int options)
{
    return item
            ? item->findPreviousItem(isKeyFocusItem, options)
            : findPreviousChild(isKeyFocusItem);
}

/*!
    Returns the first available item after \a item that is able to accept key focus and satisfies
    the search \a options.
*/
Item *Window::nextKeyFocusItem(Item *item, int options)
{
    return item
            ? item->findNextItem(isKeyFocusItem, options)
            : findNextChild(isKeyFocusItem);
}

void Window::Axis::initialize(int fd, int code, int screenSize)
{
    if (!initialized) {
        initialized = true;

        struct input_absinfo info;
        if (ioctl(fd, EVIOCGABS(code), &info) >= 0) {
            numerator = screenSize;
            denomintator = info.maximum - info.minimum;
            offset = info.minimum;
        }
    }
}

void Window::fingerPressed(int x, int y)
{
    /* On the 1st finger press: Initialize touch
     * coordinate to screen coordinate mapping.
     */
    int fd = m_multiTouch->multitouchFd();
    if (fd != -1) {
        m_touch.x.initialize(fd, ABS_MT_POSITION_X, width());
        m_touch.y.initialize(fd, ABS_MT_POSITION_Y, height());
    }

    m_touch.x.value = m_touch.x.scale(x);
    m_touch.y.value = m_touch.y.scale(y);

    log_private("#### down " << x << ","<< y << " -> " << m_touch.x.value << "," << m_touch.y.value);

    m_touch.item = findItemAt(m_touch.x.value, m_touch.y.value, [](const Item *item) -> int {
        if (!item->isEnabled() || !item->isVisible()) {
            return SkipChildren;
        }
        if (item->canActivate() || item->inputFocusOnPress() || item->keyFocusOnPress()) {
            return Match;
        }
        return 0;
    });

    if (m_touch.item) {
        bool keyFocusGiven = false;
        bool inputFocusGiven = false;
        bool pressFocusGiven = false;
        for (Item *item = m_touch.item;
             item && (!keyFocusGiven || !inputFocusGiven || !pressFocusGiven);
             item = item->m_parent) {
            if (!keyFocusGiven && item->m_keyFocusOnPress) {
                keyFocusGiven = true;
                setKeyFocusItem(item);
            }
            if (!inputFocusGiven && item->m_inputFocusOnPress) {
                inputFocusGiven = true;
                setInputFocusItem(item);

            }
            if (!pressFocusGiven && item->m_canActivate) {
                pressFocusGiven = true;
                if (m_pressedItem != item) {
                    if (m_pressedItem) {
                        m_pressedItem->invalidateFocus();
                    }
                    m_pressedItem = item;
                    m_pressedItem->invalidateFocus();
                }
                m_touch.item = item;
            }
        }
    }
}

void Window::fingerMoved(int x, int y)
{
    m_touch.x.value = m_touch.x.scale(x);
    m_touch.y.value = m_touch.y.scale(y);

    log_private("#### move " << x << ","<< y << " -> " << m_touch.x.value << "," << m_touch.y.value);

    if (m_touch.item && !m_touch.item->contains(m_touch.x.value, m_touch.y.value)) {
        if (m_touch.item == m_pressedItem) {
            m_pressedItem->invalidateFocus();
            m_pressedItem = nullptr;
        }
        m_touch.item = nullptr;
    }
}

void Window::fingerLifted(int x, int y)
{
    m_touch.x.value = m_touch.x.scale(x);
    m_touch.y.value = m_touch.y.scale(y);

    log_private("#### lift " << x << ","<< y << " -> " << m_touch.x.value << "," << m_touch.y.value);

    if (m_touch.item) {
        if (m_touch.item == m_pressedItem) {
            m_pressedItem->invalidateFocus();
            if (m_touch.item->contains(m_touch.x.value, m_touch.y.value)) {
                m_touch.item->activate();
            };
            m_pressedItem = nullptr;
        }
        m_touch.item = nullptr;
    }
}

void Window::fingerPressed(int x, int y, void *callbackData)
{
    Window *self = static_cast<Window*>(callbackData);
    self->fingerPressed(x, y);
}

void Window::fingerMoved(int x, int y, void *callbackData)
{
    Window *self = static_cast<Window*>(callbackData);
    self->fingerMoved(x, y);
}

void Window::fingerLifted(int x, int y, void *callbackData)
{
    Window *self = static_cast<Window*>(callbackData);
    self->fingerLifted(x, y);
}

/*!
    Handles a linux input \a event.
*/
void Window::inputEvent(int fd, const input_event &event)
{
    m_multiTouch->inputEvent(fd, event);

    if (event.type == EV_KEY) {
        switch (event.code) {
        case KEY_DOWN:
        case KEY_VOLUMEDOWN:
        case KEY_UP:
        case KEY_VOLUMEUP:
            if (event.value) {
                Item * const item = event.code == KEY_UP || event.code == KEY_VOLUMEUP
                        ? previousKeyFocusItem(m_keyFocusItem, Wrap)
                        : nextKeyFocusItem(m_keyFocusItem, Wrap);
                if (m_keyFocusItem != item) {
                    if (m_keyFocusItem) {
                        m_keyFocusItem->invalidateFocus();
                    }
                    m_keyFocusItem = item;
                    if (m_keyFocusItem) {
                        m_keyFocusItem->invalidateFocus();
                    }
                }
            }
            break;
        case KEY_POWER:
            if (m_itemFlags & PowerButtonDoesntSelect) {
                // Flagged to not to select, just lose focus
                if (m_keyFocusItem && m_pressedItem) {
                    m_pressedItem->invalidateFocus();
                }
                // Don't fall through to select
                break;
            }
        case KEY_ENTER:
        case KEY_OK:
        case KEY_SELECT:
            if (m_keyFocusItem) {
                if (m_pressedItem) {
                    m_pressedItem->invalidateFocus();
                }
                if (event.value) {
                    m_pressedItem = m_keyFocusItem;
                    m_pressedItem->invalidateFocus();
                } else {
                    if (m_pressedItem == m_keyFocusItem) {
                        m_pressedItem->activate();
                    }
                    m_pressedItem = nullptr;
                }
            } else {
                keyPress(KEY_ENTER, '\0');
            }
            break;
        case KEY_0: case KEY_NUMERIC_0: keyPress(event.code, '0'); break;
        case KEY_1: case KEY_NUMERIC_1: keyPress(event.code, '1'); break;
        case KEY_2: case KEY_NUMERIC_2: keyPress(event.code, '2'); break;
        case KEY_3: case KEY_NUMERIC_3: keyPress(event.code, '3'); break;
        case KEY_4: case KEY_NUMERIC_4: keyPress(event.code, '4'); break;
        case KEY_5: case KEY_NUMERIC_5: keyPress(event.code, '5'); break;
        case KEY_6: case KEY_NUMERIC_6: keyPress(event.code, '6'); break;
        case KEY_7: case KEY_NUMERIC_7: keyPress(event.code, '7'); break;
        case KEY_8: case KEY_NUMERIC_8: keyPress(event.code, '8'); break;
        case KEY_9: case KEY_NUMERIC_9: keyPress(event.code, '9'); break;
        default:
            break;
        }
    }
}

/*!
    Handles a update socket notifier event where \a data is a pointer to a Window.
*/
int Window::update_callback(int, uint32_t, void *data)
{
    Window * const window = static_cast<Window *>(data);

    if (window->m_invalidatedFlags & (State | InputFocus)) {
        window->updateItems(window->m_invalidatedFlags, true);
    }
    if (window->m_invalidatedFlags & Layout) {
        window->layoutItems();
    }
    if (window->m_invalidatedFlags & Draw) {
        window->drawItems(0, 0, 1.);
        gr_flip();
    }
    window->m_invalidatedFlags = 0;

    int64_t eventData;
    const ssize_t size = ::read(window->m_eventFd, &eventData, sizeof(eventData));
    assert(size == sizeof(eventData));

    return 0;
}

void Window::disablePowerButtonSelect()
{
    setItemFlags(itemFlags() | PowerButtonDoesntSelect);
}

/*!
    \class Sailfiish::MinUi::ResizeableItem
    \brief An item that can be freely resized.
*/

/*!
    Constructs a resizeable item as a child of \a parent.
*/
ResizeableItem::ResizeableItem(Item *parent)
    : Item(parent)
{
}

/*!
    Destroys a resizeable item.
*/
ResizeableItem::~ResizeableItem()
{
}

/*!
    \class Sailfiish::MinUi::ContainerItem
    \brief An item which child items can be added to.
*/

/*!
    Constructs a container item as a child of \a parent.
*/
ContainerItem::ContainerItem(Item *parent)
    : ResizeableItem(parent)
{
}

/*!
    Destroys a container item.
*/
ContainerItem::~ContainerItem()
{
}

/*!
    \class Sailfiish::MinUi::ActivatableItem
    \brief An item which can handle an activate event from a tap or a key press.
*/

/*!
    Constructs a activatable item as a child of \a parent.
*/
ActivatableItem::ActivatableItem(Item *parent)
    : ResizeableItem(parent)
{
    setKeyFocusOnPress(true);
    setCanActivate(true);
    setAcceptsKeyFocus(true);
}

/*!
    Destroys an activatable item.
*/
ActivatableItem::~ActivatableItem()
{
}

/*!
    Sets a \a callback which will be invoked when an item is tapped or a key press activates it.
*/
void ActivatableItem::onActivated(const std::function<void()> &callback)
{
    m_activated = callback;
}

/*!
    Invokes the onActivated() callback.
*/
void ActivatableItem::activate()
{
    if (m_activated) {
        m_activated();
    }
}

}}
