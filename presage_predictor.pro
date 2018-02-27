TEMPLATE = lib
TARGET = PresagePredictor
CONFIG += qt plugin c++11
QT += qml quick dbus
QT -= gui network

TARGET = $$qtLibraryTarget($$TARGET)
uri = hu.mm.presagepredictor

SOURCES += \
    src/presagepredictor.cpp \
    src/plugin.cpp \
    src/notificationmanager.cpp \
    src/presagepredictormodel.cpp

HEADERS += \
    src/presagepredictor.h \
    src/plugin.h \
    src/notificationmanager.h \
    src/presagepredictormodel.h

LIBS += -lpresage -lsqlite3 -lmarisa

DISTFILES += qmldir \
    rpm/PresagePredictor.yaml \
    qml/PresageInputHandler.qml \
    rpm/maliit-plugin-presage.yaml \
    rpm/maliit-plugin-presage.changes \
    rpm/maliit-plugin-presage.spec

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

# presage configuration
presageconfig.files = data/presage.xml
presageconfig.path = /etc

INSTALLS += presageconfig

# presage default empty database
presageempty.files = data/database_empty
presageempty.path = /usr/share/presage

INSTALLS += presageempty
