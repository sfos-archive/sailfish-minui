SAILFISH_SOURCE_ROOT = $$PWD
SAILFISH_BUILD_ROOT = $$clean_path($$relative_path($$shadowed($$PWD), $${OUT_PWD}))

INCLUDEPATH += \
    $$SAILFISH_SOURCE_ROOT/src/3rdparty \
    $$SAILFISH_SOURCE_ROOT/src

CONFIG += link_pkgconfig
