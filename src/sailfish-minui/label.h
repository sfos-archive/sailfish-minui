/*
 * Copyright (C) 2017-2019 Jolla Ltd.
 *
 * License: Proprietary
 */

#ifndef SAILFISH_MINUI_LABEL_H
#define SAILFISH_MINUI_LABEL_H

#include <sailfish-minui/item.h>

namespace Sailfish { namespace MinUi {

class Label : public Item
{
public:
    explicit Label(const char *name, Item *parent = nullptr);
    ~Label();

    bool isValid() const { return m_text; }

    Color color() const { return m_color; }
    void setColor(Color color);

protected:
    void draw(int x, int y, double opacity) override;

private:
    gr_surface m_text = nullptr;
    Color m_color;
};

class LiteralLabel : public Item
{
public:
    explicit LiteralLabel(std::string text, Item *parent = nullptr);
    ~LiteralLabel() {}

    Color color() const { return m_color; }
    void setColor(Color color);

    void setText(std::string text);
protected:
    void draw(int x, int y, double opacity) override;

private:
    std::string m_text;
    Color m_color;
    int m_fontWidth;
    int m_fontHeight;
};


}}

#endif
