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
