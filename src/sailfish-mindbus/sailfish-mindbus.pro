TARGET = sailfish-mindbus

PKGCONFIG += dbus-1

CONFIG += c++11

PUBLIC_HEADERS += \
    connection.h \
    message.h \
    object.h \
    pendingcall.h \
    pointer.h \
    signal.h

SOURCES += \
    connection.cpp \
    object.cpp \
    pendingcall.cpp \
    signal.cpp

include (../../sailfish-minui-lib.pri)

QMAKE_PKGCONFIG_NAME = sailfish-mindbus
QMAKE_PKGCONFIG_DESCRIPTION = Minimal DBus C++ library
QMAKE_PKGCONFIG_LIBDIR = $$target.path
QMAKE_PKGCONFIG_INCDIR = /usr/include
QMAKE_PKGCONFIG_DESTDIR = pkgconfig
QMAKE_PKGCONFIG_VERSION = $$VERSION
QMAKE_PKGCONFIG_REQUIRES = dbus-1
