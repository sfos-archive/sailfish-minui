/****************************************************************************
**
** Copyright (C) 2017 Jolla Ltd.
** Contact: Andrew den Exter <andrew.den.exter@jolla.com>
**
****************************************************************************/

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
    MinUi::Page * const m_page;
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
        m_textField.onAccepted([this](const std::string &) {
            m_textField.setText(std::string());
        });
        m_textField.onCanceled([this]() {
            m_textField.setText("");
        });
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
        m_keypad.align(MinUi::Bottom, *this, MinUi::Bottom);
        m_keypad.centerBetween(*this, MinUi::Left, *this, MinUi::Right);
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
    }

};

int main(int, char *[])
{
    MinUi::EventLoop eventLoop;

    Gallery gallery(&eventLoop);

    return eventLoop.execute();
}
