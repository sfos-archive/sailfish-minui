TEMPLATE = app
TARGET = sailfish-minui-gallery

include ($$PWD/../../sailfish-minui-common.pri)

CONFIG -= qt

DESTDIR = $$SAILFISH_BUILD_ROOT/bin

SOURCES +=  \
    main.cpp

LIBS += \
    -L$$SAILFISH_BUILD_ROOT/lib -lsailfish-minui

headers.ids = \
    sailfish-minui-gallery-he-controls \
    sailfish-minui-gallery-he-input \
    sailfish-minui-gallery-he-gallery
headers.heading = true
headers.size = Large
headers.alignment = Right

menuitems.ids = \
    sailfish-minui-gallery-me-menus \
    sailfish-minui-gallery-me-controls \
    sailfish-minui-gallery-me-input \
    sailfish-minui-gallery-me-back \
    sailfish-minui-gallery-me-close \
    sailfish-minui-gallery-la-text-field \
    sailfish-minui-gallery-la-password-field

menuitems.heading = false
menuitems.size = Medium
menuitems.alignment = Left

include($$SAILFISH_SOURCE_ROOT/src/sailfish-minui-label-tool/sailfish-minui-resources.prf)

SAILFISH_MINUI_TRANSLATIONS = \
    headers \
    menuitems

target.path = /usr/bin

INSTALLS += \
    target
