/*
 * Copyright (C) 2017-2019 Jolla Ltd.
 *
 * License: Proprietary
 */

#ifndef SAILFISH_MINUI_TEXTINPUT_H
#define SAILFISH_MINUI_TEXTINPUT_H

#include <sailfish-minui/item.h>

namespace Sailfish { namespace MinUi {

class TextInput : public ResizeableItem
{
public:
    enum Reason {
        Assignment,
        Insertion,
        Deletion,
        Accepted,
        Canceled
    };

    explicit TextInput(Item *parent = nullptr);
    ~TextInput();

    std::string text() const { return m_text; }
    void setText(const std::string &text);

    Color color() const { return m_color; }
    void setColor(Color color);

    bool isBold() const { return m_bold; }
    void setBold(bool bold);

    int maximumLength() const { return m_maximumLength; }
    void setMaximumLength(int length);

    HorizontalAlignment horizontalAlignment() const { return m_horizontalAlignment; }
    void setHorizontalAlignment(HorizontalAlignment alignment);

    int leftMargin() const { return m_leftMargin; }
    void setLeftMargin(int margin);

    int rightMargin() const { return m_rightMargin; }
    void setRightMargin(int margin);

    void backspace();

    void onTextChanged(const std::function<void(TextInput::Reason reason)> &callback);
    void onAccepted(const std::function<void(const std::string &text)> &callback);
    void onCanceled(const std::function<void()> &callback);

    using Item::setInputFocusOnPress;
    using Item::acceptsInputFocus;

protected:
    virtual std::string displayText(const std::string &text);
    virtual void textChanged(Reason reason);

    void draw(int dx, int dy, double opacity) override;
    void updateState(bool enabled) override;

    bool keyPress(int code, const char character) override;

private:
    std::function<void(const std::string &text)> m_accepted;
    std::function<void()> m_canceled;
    std::function<void(Reason reason)> m_textChanged;
    std::string m_text;
    std::string m_displayText;
    Color m_color;
    int m_maximumLength = 4096;
    int m_fontWidth;
    int m_fontHeight;
    int m_leftMargin = 0;
    int m_rightMargin = 0;
    HorizontalAlignment m_horizontalAlignment = HorizontalAlignment::Left;
    bool m_bold = false;
};

class PasswordInput : public TextInput
{
public:
    explicit PasswordInput(Item *parent = nullptr);
    ~PasswordInput();

    int echoDelay() const { return m_echoDelay; }
    void setEchoDelay(int delay);

    bool maskingEnabled() const { return m_maskingEnabled; }
    void setMaskingEnabled(bool enabled);

protected:
    std::string displayText(const std::string &text) override;
    void textChanged(Reason reason) override;

private:
    inline void finishEcho();

    int m_echoDelay = 100;
    int m_maskTimerId = 0;
    bool m_maskingEnabled = true;
};

}}

#endif
