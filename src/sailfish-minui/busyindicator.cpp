/*
 * Copyright (C) 2019 Jolla Ltd.
 *
 * License: Proprietary
 */

#include "busyindicator.h"
#include "eventloop.h"

#include <iostream>

#define MIN_STATE 1
#define MAX_STATES 8

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
        createNext();

        m_runningId = eventLoop()->createTimer(125, [this]() {
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
    if (m_indicator) {
        delete m_indicator;
        m_indicator = nullptr;
    }

    if (m_currentState == 0) {
        m_currentState = MAX_STATES;
    }

    setVisible(true);

    char image[32];
    sprintf(image, "graphic-busyindicator-medium-%d", m_currentState);

    m_indicator = new Icon(image, this);
    m_indicator->setColor(m_color);
    m_indicator->centerIn(*this);

    --m_currentState;
}

}}
