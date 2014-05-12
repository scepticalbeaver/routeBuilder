include(../globals.pri)

TRIKCONTROL_DIR = $$PWD/../trikRuntime/trikControl

EMULATORS_BINDIR = $$PWD/../bin
EMULATORS_DIR = $$PWD/../emulatorTest


QT       += core
QT       += network
QT       += gui

TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG += qt
QMAKE_CXXFLAGS += -std=c++11

TARGET = routeBuilder

DEPENDPATH += . $$EMULATORS_DIR
INCLUDEPATH = \
	$$PWD \
	$$BOOST_DIR \
	$$EMULATORS_DIR \
	$$TRIKCONTROL_DIR/include \

#LIBS += -L$$EMULATORS_BINDIR -lemtest -L$$TRIKCONTROL_BINDIR -ltrikControl
LIBS += -L$$EMULATORS_BINDIR -lemtest -L$$TRIKCONTROL_BINDIR -ltrikControl-x86-d

!macx {
    QMAKE_LFLAGS += -Wl,-O1,-rpath,.
    QMAKE_LFLAGS += -Wl,-rpath-link,$$DESTDIR
}

unix: {
    QMAKE_POST_LINK =  "cp -rf $$TRIKCONTROL_BINDIR/libtrikControl* $$DESTDIR"
}


SOURCES += src/main.cpp \
    src/trackStorage.cpp \
    src/interactiveCommander.cpp \
    src/routeController.cpp \
    src/routeRepeater.cpp \
    src/motorComplect.cpp \
    src/deviceExplorer.cpp \
    src/wrappers/motorWrap.cpp \
    src/wrappers/encoderWrap.cpp \
    src/wrappers/robotWrap.cpp

HEADERS += \
    src/trackStorage.h \
    src/interactiveCommander.h \
    src/routeController.h \
    src/routeRepeater.h \
    src/motorComplect.h \
    src/deviceExplorer.h \
    src/wrappers/motorWrap.h \
    src/wrappers/encoderWrap.h \
    src/wrappers/robotWrap.h

