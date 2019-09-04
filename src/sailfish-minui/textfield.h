/*
 * Copyright (C) 2017-2019 Jolla Ltd.
 *
 * License: Proprietary
 */

#ifndef SAILFISH_MINUI_TEXTFIELD_H
#define SAILFISH_MINUI_TEXTFIELD_H

#include <sailfish-minui/button.h>
#include <sailfish-minui/rectangle.h>
#include <sailfish-minui/textinput.h>

namespace Sailfish { namespace MinUi {

template <typename Input> class TextFieldTemplate : public ResizeableItem
{
public:
    explicit TextFieldTemplate(const char *placeholder, Item *parent = nullptr);
    ~TextFieldTemplate();

    Palette palette() const { return m_backspace.palette(); }
    void setPalette(const Palette &palette);

    std::string text() const { return m_input.text(); }
    void setText(const std::string &text) { m_input.setText(text); }

    bool isBold() const { return m_input.isBold(); }
    void setBold(bool bold) { m_input.setBold(bold); }

    bool isBorderVisible() const { return m_underline.isVisible(); }
    void setBorderVisible(bool visible) { m_underline.setVisible(visible); }

    int maximumLength() const { return m_input.maximumLength(); }
    void setMaximumLength(int length) { m_input.setMaximumLength(length); }

    HorizontalAlignment horizontalAlignment() const { return m_input.horizontalAlignment(); }
    void setHorizontalAlignment(HorizontalAlignment alignment);

    void onTextChanged(const std::function<void(TextInput::Reason reason)> &callback) { m_input.onTextChanged(callback); }
    void onAccepted(const std::function<void(const std::string &text)> &callback) { m_input.onAccepted(callback); }
    void onCanceled(const std::function<void()> &callback) { m_input.onCanceled(callback); }

    using Item::setAcceptsKeyFocus;
    using Item::setKeyFocusOnPress;
    using Item::setCanActivate;

protected:
    void activate() override;
    void layout() override;
    void updateState(bool enabled) override;

    Input m_input { this };

private:
    inline void setForegroundColor(Color color);

    Label m_placeholder;
    IconButton m_backspace { "icon-m-backspace", this };
    Rectangle m_underline {  this };
};

class TextField : public TextFieldTemplate<TextInput>
{
public:
    explicit TextField(Item *parent = nullptr) : TextField(nullptr, parent) {}
    explicit TextField(const char *placeholder, Item *parent = nullptr);
    ~TextField();
};

class PasswordField : public TextFieldTemplate<PasswordInput>
{
public:
    explicit PasswordField(Item *parent = nullptr) : PasswordField(nullptr, parent) {}
    explicit PasswordField(const char *placeholder, Item *parent = nullptr);
    ~PasswordField();

    int echoDelay() const { return m_input.echoDelay(); }
    /* Character masking delay, use -1 to disable */
    void setEchoDelay(int delay) { m_input.setEchoDelay(delay); }
};

}}

#endif
