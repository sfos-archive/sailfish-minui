TARGET = sailfish-minui

CONFIG += c++11

PUBLIC_HEADERS += \
    eventloop.h \
    ui.h

SOURCES +=  \
    eventloop.cpp \
    ui.cpp

include (../../sailfish-minui-lib.pri)

PKGCONFIG += minui

QMAKE_PKGCONFIG_NAME = sailfish-minui
QMAKE_PKGCONFIG_DESCRIPTION = Minimal UI C++ library
QMAKE_PKGCONFIG_LIBDIR = $$target.path
QMAKE_PKGCONFIG_INCDIR = /usr/include
QMAKE_PKGCONFIG_DESTDIR = pkgconfig
QMAKE_PKGCONFIG_VERSION = $$VERSION
QMAKE_PKGCONFIG_REQUIRES = minui
