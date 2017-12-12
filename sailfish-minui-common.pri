SAILFISH_SOURCE_ROOT = $$PWD
SAILFISH_BUILD_ROOT = $$clean_path($$relative_path($$shadowed($$PWD), $${OUT_PWD}))

SAILFISH_MINUI_LABEL_TOOL = $$SAILFISH_BUILD_ROOT/bin/sailfish-minui-label-tool

INCLUDEPATH += \
    $$SAILFISH_SOURCE_ROOT/src

CONFIG += \
    link_pkgconfig

QMAKE_CXXFLAGS += \
    -Werror
