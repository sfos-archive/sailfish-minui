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

#include <sailfish-minui/eventloop.h>
#include <sailfish-minui/ui.h>

using namespace Sailfish;

class ControlsPage : public MinUi::Item
{
    //% "Controls"
    MinUi::PageHeader m_pageHeader { "icon-m-back", qtTrId("sailfish-minui-gallery-he-controls"), this };
    MinUi::ProgressBar m_animatedProgress { this };
    MinUi::ProgressBar m_buttonProgress { this };
    MinUi::IconButton m_decrementButton { "icon-m-left", this };
    MinUi::IconButton m_incrementButton { "icon-m-right", this };
    MinUi::BusyIndicator m_busyIndicator { this };
    MinUi::Page * const m_page;
    int m_animationId = 0;
public:
    explicit ControlsPage(MinUi::Page *page)
        : Item(page)
        , m_page(page)
    {
        m_pageHeader.onBackActivated([this]() {
            m_page->remove();
        });
        m_decrementButton.onActivated([this]() {
            m_buttonProgress.setValue(m_buttonProgress.value() - 0.1);
            invalidate(State);
        });
        m_incrementButton.onActivated([this]() {
            m_buttonProgress.setValue(m_buttonProgress.value() + 0.1);
            invalidate((State));
        });
        m_animationId = eventLoop()->createTimer(200, [this]() {
            m_animatedProgress.setValue(m_animatedProgress.value() < 1.
                    ? m_animatedProgress.value() + 0.05
                    : 0.);
        });

        m_busyIndicator.setRunning(true);

        invalidate(State);
    }

    ~ControlsPage()
    {
        if (m_animationId > 0) {
            eventLoop()->cancelTimer(m_animationId);
        }
    }

protected:
    void updateState(bool) override
    {
        m_decrementButton.setEnabled(m_buttonProgress.value() > 0.);
        m_incrementButton.setEnabled(m_buttonProgress.value() < 1.);
    }

    void layout() override
    {
        fill(*m_page);

        m_pageHeader.align(MinUi::Top, *this, MinUi::Top);
        m_animatedProgress.align(MinUi::Top, m_pageHeader, MinUi::Bottom);
        m_animatedProgress.centerBetween(*this, MinUi::Left, *this, MinUi::Right);
        m_buttonProgress.align(MinUi::Top, m_animatedProgress, MinUi::Bottom);
        m_buttonProgress.centerBetween(*this, MinUi::Left, *this, MinUi::Right);
        m_decrementButton.align(MinUi::Top, m_buttonProgress, MinUi::Bottom);
        m_decrementButton.align(MinUi::HorizontalCenter, m_buttonProgress, MinUi::Left);
        m_incrementButton.align(MinUi::Top, m_buttonProgress, MinUi::Bottom);
        m_incrementButton.align(MinUi::HorizontalCenter, m_buttonProgress, MinUi::Right);
        m_busyIndicator.centerBetween(*this, MinUi::Left, *this, MinUi::Right);
        m_busyIndicator.align(MinUi::Top, m_incrementButton, MinUi::Bottom);
    }
};

class InputPage : public MinUi::Item
{
    //% "Input"
    MinUi::PageHeader m_pageHeader { "icon-m-back", qtTrId("sailfish-minui-gallery-he-input"), this };
    //% "Password field"
    MinUi::PasswordField m_passwordField { qtTrId("sailfish-minui-gallery-la-password-field"), this };
    //% "Text field"
    MinUi::TextField m_textField { qtTrId("sailfish-minui-gallery-la-text-field"), this };
    MinUi::Keypad m_keypad { this };
    MinUi::Keyboard m_keyboard { this };

    MinUi::Page * const m_page;
    MinUi::IconButton m_keypadButton { "icon-m-dialpad", this };
    MinUi::IconButton m_keyboardButton { "icon-m-keyboard", this };

public:
    explicit InputPage(MinUi::Page *page)
        : Item(page)
        , m_page(page)
    {
        m_pageHeader.onBackActivated([this]() {
            m_page->remove();
        });
        m_passwordField.onAccepted([this](const std::string &) {
            m_passwordField.setText(std::string());
        });
        m_passwordField.onCanceled([this]() {
            m_passwordField.setText("");
        });
        // TODO: should update on focus changes too, but there's no callback currently for that.
        // Not adding just for test app yet.
        m_passwordField.onTextChanged([this](MinUi::TextInput::Reason) { updateInputAcceptance (); });
        m_textField.onTextChanged([this](MinUi::TextInput::Reason) { updateInputAcceptance (); });
        m_textField.onAccepted([this](const std::string &) {
            m_textField.setText(std::string());
        });
        m_textField.onCanceled([this]() {
            m_textField.setText("");
        });
        m_keyboardButton.onActivated([this]() {
            m_keyboard.setVisible(true);
            m_keypad.setVisible(false);
            m_passwordField.setExtraButtonMode(MinUi::PasswordField::ShowTextVisibilityToggle);
        });
        m_keypadButton.onActivated([this]() {
            m_keyboard.setVisible(false);
            m_keypad.setVisible(true);
            m_passwordField.setExtraButtonMode(MinUi::PasswordField::ShowBackspace);
        });

        m_keyboard.setEnterEnabled(false);
        m_keyboard.setVisible(false);
    }

protected:
    void layout() override
    {
        fill(*m_page);

        m_pageHeader.align(MinUi::Top, *this, MinUi::Top);

        m_passwordField.align(MinUi::Top, m_pageHeader, MinUi::Bottom);
        m_passwordField.horizontalFill(*this, MinUi::theme.horizontalPageMargin);
        m_textField.align(MinUi::Top, m_passwordField, MinUi::Bottom);
        m_textField.horizontalFill(*this, MinUi::theme.horizontalPageMargin);
        m_textField.align(MinUi::Top, m_passwordField, MinUi::Bottom);

        m_keypadButton.align(MinUi::Top, m_textField, MinUi::Bottom);
        m_keypadButton.align(MinUi::Left, *this, MinUi::Left, MinUi::theme.horizontalPageMargin);
        m_keyboardButton.align(MinUi::Top, m_keypadButton, MinUi::Top);
        m_keyboardButton.align(MinUi::Left, m_keypadButton, MinUi::Right, MinUi::theme.paddingLarge);

        m_keypad.align(MinUi::Bottom, *this, MinUi::Bottom);
        m_keypad.centerBetween(*this, MinUi::Left, *this, MinUi::Right);
        m_keyboard.align(MinUi::Bottom, *this, MinUi::Bottom);
        m_keyboard.horizontalFill(*this);
    }

    void updateInputAcceptance()
    {
        bool enabled;
        if (m_passwordField.hasActiveInputFocus()) {
            enabled = m_passwordField.text().length() > 0;
        } else {
            enabled = m_textField.text().length() > 0;
        }
        m_keypad.setAcceptEnabled(enabled);
        m_keyboard.setEnterEnabled(enabled);
    }
};

class MenuPage : public MinUi::Menu
{
protected:
    //% "Gallery"
    MinUi::PageHeader m_pageHeader { "icon-m-clear", qtTrId("sailfish-minui-gallery-he-gallery"), this };
    //% "Controls"
    MinUi::MenuItem m_controlsItem { "icon-m-gesture", qtTrId("sailfish-minui-gallery-me-controls"), this };
    //% "Input"
    MinUi::MenuItem m_inputItem { "icon-m-dialpad", qtTrId("sailfish-minui-gallery-me-input"), this };

    MinUi::Page * const m_page;

public:
    explicit MenuPage(MinUi::Page *page)
        : Menu(page)
        , m_page(page)
    {
        m_pageHeader.onBackActivated([this]() {
            eventLoop()->exit();
        });
        m_controlsItem.onActivated([this]() {
            m_page->pageStack()->pushNew<ControlsPage>();
        });
        m_inputItem.onActivated([this]() {
            m_page->pageStack()->pushNew<InputPage>();
        });
    }

protected:
    void layout() override
    {
        setWidth(m_page->width());

        Menu::layout();
    }
};

class Gallery : public MinUi::Window
{
    MinUi::PageStack m_pageStack { this };
public:
    explicit Gallery(MinUi::EventLoop *loop)
        : MinUi::Window(loop)
    {
        m_pageStack.fill(*this);
        m_pageStack.pushNew<MenuPage>();
        setKeyFocusItem(&m_pageStack);
        disablePowerButtonSelect();
    }

};

int main(int, char *[])
{
    MinUi::EventLoop eventLoop;

    Gallery gallery(&eventLoop);

    return eventLoop.execute();
}
