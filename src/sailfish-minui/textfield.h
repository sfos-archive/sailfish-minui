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
    void backspace() { m_input.backspace(); }

    bool isBold() const { return m_input.isBold(); }
    void setBold(bool bold) { m_input.setBold(bold); }

    bool isBorderVisible() const { return m_underline.isVisible(); }
    void setBorderVisible(bool visible) { m_underline.setVisible(visible); }

    // not ideal api, but otherwise there's no way to know whether the child item has active focus
    bool hasActiveInputFocus() { return m_input.hasInputFocus(); }

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
    IconButton m_backspace {"icon-m-backspace", this};

private:
    inline void setForegroundColor(Color color);

    Label m_placeholder;
    Rectangle m_underline {  this };
};

class TextField : public TextFieldTemplate<TextInput>
{
public:
    explicit TextField(Item *parent = nullptr) : TextField(nullptr, parent) {}
    explicit TextField(const char *placeholder, Item *parent = nullptr);
    ~TextField();

    // TODO: allow to control backspace visibility once there's need to use keyboard with TextField
protected:
    void updateState(bool enabled) override;
};

class PasswordField : public TextFieldTemplate<PasswordInput>
{
public:
    enum ExtraButtonMode {
        ShowBackspace,
        ShowTextVisibilityToggle
    };

    explicit PasswordField(Item *parent = nullptr) : PasswordField(nullptr, parent) {}
    explicit PasswordField(const char *placeholder, Item *parent = nullptr);
    ~PasswordField();

    /* Character masking delay */
    int echoDelay() const { return m_input.echoDelay(); }
    void setEchoDelay(int delay) { m_input.setEchoDelay(delay); }

    bool maskingEnabled() const { return m_input.maskingEnabled(); }
    void setMaskingEnabled(bool enabled) { m_input.setMaskingEnabled(enabled); }

    void setExtraButtonMode(ExtraButtonMode mode);

protected:
    void layout() override;
    void updateState(bool enabled) override;

private:
    void toggleTextVisibility();

    IconButton m_showTextButton {"icon-splus-show-password", this};
    IconButton m_hideTextButton {"icon-splus-hide-password", this};
    ExtraButtonMode m_extraButtonMode = ShowBackspace;
};

}}

#endif
