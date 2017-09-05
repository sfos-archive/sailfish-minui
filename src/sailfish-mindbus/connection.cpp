/****************************************************************************
**
** Copyright (C) 2017 Jolla Ltd.
** Contact: Andrew den Exter <andrew.den.exter@jolla.com>
**
****************************************************************************/

#include "connection.h"
#include <string.h>

namespace Sailfish { namespace MinDBus {

Connection systemBus()
{
    return dbus_bus_get(DBUS_BUS_SYSTEM, nullptr);
}

}}
