TARGET = sailfish-minui-dbus

PKGCONFIG += \
    dbus-1 \
    minui

PUBLIC_HEADERS += \
    eventloop.h

SOURCES +=  \
    eventloop.cpp

include (../../sailfish-minui-lib.pri)

LIBS += \
    -lsailfish-minui \
    -lsailfish-mindbus

QMAKE_PKGCONFIG_NAME = sailfish-minui-dbus
QMAKE_PKGCONFIG_DESCRIPTION = Minimal UI C++ library dbus event loop
QMAKE_PKGCONFIG_LIBDIR = $$target.path
QMAKE_PKGCONFIG_INCDIR = /usr/include
QMAKE_PKGCONFIG_DESTDIR = pkgconfig
QMAKE_PKGCONFIG_VERSION = $$VERSION
QMAKE_PKGCONFIG_REQUIRES = sailfish-mindbus = $$VERSION sailfish-minui = $$VERSION
