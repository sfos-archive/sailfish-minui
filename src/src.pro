TEMPLATE = subdirs

SUBDIRS += \
    gallery \
    sailfish-mindbus \
    sailfish-minui \
    sailfish-minui-dbus \
    sailfish-minui-label-tool

gallery.depends = \
    sailfish-minui \
    sailfish-minui-label-tool

sailfish-minu.depends = \
    sailfish-minui-label-tool

sailfish-minui-dbus.depends = \
    sailfish-mindbus sailfish-minui
