# NOTICE:
#
# Application name defined in TARGET has a corresponding QML filename.
# If name defined in TARGET is changed, the following needs to be done
# to match new name:
#   - corresponding QML filename must be changed
#   - desktop icon filename must be changed
#   - desktop filename must be changed
#   - icon definition filename in desktop file must be changed
#   - translation filenames have to be changed

# The name of your application
TARGET = jollapresageplugin
QT += qml
QT -= gui


OBJECTS_DIR = build
MOC_DIR = build
UI_DIR = build
RCC_DIR=build

DESTDIR = bin


TEMPLATE = lib
CONFIG += qt plugin


# this line will install the qml file to the maliit path
qmldir.path = /usr/share/maliit/plugins/com/jolla
qmldir.files += $$_PRO_FILE_PWD_/qml/PresageInputHandler.qml
INSTALLS += qmldir

# and this one will install the library
target.path = /usr/lib/qt5/qml/com/jolla
INSTALLS += target

SOURCES += \
    src/presagemodel.cpp \
    src/presagepredictor.cpp

OTHER_FILES += \
    rpm/presageInputHandler.spec

HEADERS += \
    src/presagemodel.h \
    src/presageplugin.h \
    src/presagepredictor.h

LIBS += -lpresage

DISTFILES += \
    qml/PresageInputHandler.qml \
    rpm/jollapresageplugin.yaml \
    rpm/jollapresageplugin.changes.in \
    rpm/jollapresageplugin.spec


