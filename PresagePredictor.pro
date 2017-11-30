TARGET = PresagePredictor
QT += qml dbus
QT -= gui network


OBJECTS_DIR = build
MOC_DIR = build
UI_DIR = build
RCC_DIR=build
DESTDIR = bin

uri = hu.mm.presagepredictor

TEMPLATE = lib
CONFIG += qt plugin

SOURCES += \
    src/presagepredictor.cpp \
    src/plugin.cpp \
    src/notificationmanager.cpp

HEADERS += \
    src/presagepredictor.h \
    src/plugin.h \
    src/notificationmanager.h

LIBS += -lpresage

DISTFILES += qmldir \
    rpm/PresagePredictor.yaml \
    qml/PresageInputHandler.qml

!equals(_PRO_FILE_PWD_, $$OUT_PWD) {
    copy_qmldir.target = $$OUT_PWD/qmldir
    copy_qmldir.depends = $$_PRO_FILE_PWD_/qmldir
    copy_qmldir.commands = $(COPY_FILE) \"$$replace(copy_qmldir.depends, /, $$QMAKE_DIR_SEP)\" \"$$replace(copy_qmldir.target, /, $$QMAKE_DIR_SEP)\"
    QMAKE_EXTRA_TARGETS += copy_qmldir
    PRE_TARGETDEPS += $$copy_qmldir.target
}

qmldir.files = qmldir
installPath = $$[QT_INSTALL_QML]/$$replace(uri, \\., /)
qmldir.path = $$installPath
target.path = $$installPath

inputhandler.files = $$_PRO_FILE_PWD_/qml/PresageInputHandler.qml
inputhandler.path = /usr/share/maliit/plugins/com/jolla

INSTALLS += target qmldir inputhandler

