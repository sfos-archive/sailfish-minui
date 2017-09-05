TEMPLATE = app
TARGET = sailfish-minui-label-tool

CONFIG += c++11

PKGCONFIG += \
        sailfishsilica

SOURCES += \
        main.cpp

include (../../sailfish-minui-common.pri)

DESTDIR = $$SAILFISH_BUILD_ROOT/bin

features.files = \
    sailfish-minui-resources.prf
features.path = $$[QT_INSTALL_DATA]/mkspecs/features

target.path = /usr/bin

INSTALLS += features target
