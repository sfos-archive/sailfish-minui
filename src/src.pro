TEMPLATE = subdirs

SUBDIRS += \
    sailfish-mindbus \
    sailfish-minui \
    sailfish-minui-dbus \
    sailfish-minui-label-tool

sailfish-minui-dbus.depends = sailfish-mindbus sailfish-minui
