SAILFISH_SOURCE_ROOT = $$PWD
SAILFISH_BUILD_ROOT = $$clean_path($$relative_path($$shadowed($$PWD), $${OUT_PWD}))

SAILFISH_MINUI_LABEL_TOOL = $$SAILFISH_BUILD_ROOT/bin/sailfish-minui-label-tool

INCLUDEPATH += \
    $$SAILFISH_SOURCE_ROOT/src

CONFIG += \
    link_pkgconfig

QMAKE_CXXFLAGS += \
    -Werror \
    -Wall

# Enable/disable debug logging with possible privacy/security concerns
#
# For example: While getting finger press coordinates logged during
# sw development can be useful, it can also be used for determining
# pin codes other secrets -> When building official packages, it must
# be disabled at compile time so that there is no way to turn it on
# dynamically during runtime.
#
# The value MUST remain zero in master source tree.
DEFINES += LOGGING_ENABLE_PRIVATE=0

# Enable/disable debug logging without privacy/security concerns
#
# The value SHOULD remain zero in master source tree.
DEFINES += LOGGING_ENABLE_DEBUG=0
