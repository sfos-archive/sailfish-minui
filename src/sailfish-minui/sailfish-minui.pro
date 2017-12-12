TARGET = sailfish-minui

PUBLIC_HEADERS += \
    button.h \
    eventloop.h \
    icon.h \
    image.h \
    item.h \
    keypad.h \
    label.h \
    linkedlist.h \
    menu.h \
    pagestack.h \
    progressbar.h \
    rectangle.h \
    textfield.h \
    textinput.h \
    ui.h

SOURCES +=  \
    button.cpp \
    eventloop.cpp \
    icon.cpp \
    image.cpp \
    item.cpp \
    keypad.cpp \
    label.cpp \
    menu.cpp \
    pagestack.cpp \
    progressbar.cpp \
    rectangle.cpp \
    textfield.cpp \
    textinput.cpp

keypadbuttons.ids = \
    sailfish-minui-bt-accept \
    sailfish-minui-bt-cancel
keypadbuttons.heading = false
keypadbuttons.size = Large
keypadbuttons.alignment = Center

SAILFISH_MINUI_TRANSLATIONS = \
    keypadbuttons

include (../../sailfish-minui-lib.pri)
include($$SAILFISH_SOURCE_ROOT/src/sailfish-minui-label-tool/sailfish-minui-resources.prf)

PKGCONFIG += minui

QMAKE_PKGCONFIG_NAME = sailfish-minui
QMAKE_PKGCONFIG_DESCRIPTION = Minimal UI C++ library
QMAKE_PKGCONFIG_LIBDIR = $$target.path
QMAKE_PKGCONFIG_INCDIR = /usr/include
QMAKE_PKGCONFIG_DESTDIR = pkgconfig
QMAKE_PKGCONFIG_VERSION = $$VERSION
QMAKE_PKGCONFIG_REQUIRES = minui
