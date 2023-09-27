/****************************************************************************************
** Copyright (c) 2019 - 2023 Jolla Ltd.
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

#include "busyindicator.h"
#include "eventloop.h"

#include <iostream>

#define MIN_STATE 1
#define MAX_STATES 24

namespace Sailfish { namespace MinUi {

BusyIndicator::BusyIndicator(Item *parent)
    : Item(parent)
    , m_runningId(0)
    , m_currentState(MAX_STATES)
    , m_indicator(nullptr)
{
    invalidate(State | Layout);
    setWidth(theme.iconSizeMedium + 2 * theme.paddingMedium);
    setHeight(width());
    setVisible(false);
}

BusyIndicator::~BusyIndicator()
{
    if (isRunning()) {
        eventLoop()->cancelTimer(m_runningId);
        m_runningId = 0;
    }
    delete m_indicator;
    m_indicator = nullptr;
}

bool BusyIndicator::isRunning() const
{
    return m_runningId > 0;
}

void BusyIndicator::setRunning(bool running)
{
    if (running) {
        start();
    } else {
        stop();
    }
}

void BusyIndicator::stop()
{
    if (isRunning()) {
        eventLoop()->cancelTimer(m_runningId);
        m_runningId = 0;

        delete m_indicator;
        m_indicator = nullptr;

        m_currentState = MAX_STATES;

        invalidate(State | Layout);
        setVisible(false);
    }
}

void BusyIndicator::start()
{
    if (!isRunning()) {
        invalidate(State | Layout);
        setVisible(true);
        createNext();

        m_runningId = eventLoop()->createTimer(60, [this]() {
            createNext();
        });
    }
}

Color BusyIndicator::color() const
{
    return m_color;
}

void BusyIndicator::setColor(Color color)
{
    m_color = color;
    invalidate(Draw);
}

void BusyIndicator::createNext()
{
    if (!isVisible()) {
        return;
    }

    if (m_indicator) {
        delete m_indicator;
        m_indicator = nullptr;
    }

    if (m_currentState == 0) {
        m_currentState = MAX_STATES;
    }

    char image[32];
    sprintf(image, "graphic-busyindicator-medium-%d", m_currentState);

    m_indicator = new Icon(image, this);
    m_indicator->setColor(m_color);
    m_indicator->centerIn(*this);
    appendChild(m_indicator);

    --m_currentState;
}

}}
