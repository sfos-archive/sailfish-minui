/*
 * Copyright (c) 2017-2019 Jolla Ltd.
 *
 * License: Proprietary
 */

#include "connection.h"
#include <string.h>

namespace Sailfish { namespace MinDBus {

Connection systemBus()
{
    return dbus_bus_get(DBUS_BUS_SYSTEM, nullptr);
}

}}
