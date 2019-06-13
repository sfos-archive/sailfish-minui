/****************************************************************************
**
** Copyright (C) 2019 Jolla Ltd.
**
****************************************************************************/

#ifndef SAILFISH_MINUI_BUSYINDICATOR_H
#define SAILFISH_MINUI_BUSYINDICATOR_H

#include <sailfish-minui/icon.h>

namespace Sailfish { namespace MinUi {

class BusyIndicator : public Item
{
public:
    BusyIndicator(Item *parent);
    virtual ~BusyIndicator();

    bool isRunning() const;
    void setRunning(bool running);
    void stop();
    void start();

    Color color() const;
    void setColor(Color color);

private:
    void createNext();

    int m_runningId;
    int m_currentState;
    Icon *m_indicator;
    Color m_color;
};

}}
#endif // SAILFISH_MINUI_BUSYINDICATOR
