include ($$PWD/sailfish-minui-common.pri)

TEMPLATE = lib

CONFIG -= qt
CONFIG += \
        static \
        create_pc \
        create_prl \
        no_install_prl

DESTDIR = $$SAILFISH_BUILD_ROOT/lib

HEADERS += \
        $$PUBLIC_HEADERS

isEmpty(SAILFISH_HEADER_PREFIX): SAILFISH_HEADER_PREFIX = $$relative_path($$_PRO_FILE_PWD_, $$SAILFISH_SOURCE_ROOT/src)
headers.files = $$PUBLIC_HEADERS
headers.path = /usr/include/$$SAILFISH_HEADER_PREFIX

target.path = $$[QT_INSTALL_LIBS]

INSTALLS += \
    headers \
    target
