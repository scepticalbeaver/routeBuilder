TRIKCONTROL_BINDIR = $$PWD/trikRuntime/bin/x86-debug
TRIKCONTROL_DIR = $$PWD/trikRuntime/trikControl

QT += core

TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG += qt
QMAKE_CXXFLAGS += -std=c++11


TARGET = routeBuilder

INCLUDEPATH = \
	$$PWD \
	$$TRIKCONTROL_DIR/include \

LIBS += -L$$TRIKCONTROL_BINDIR -ltrikControl-x86-d

!macx {
	QMAKE_LFLAGS += -Wl,-O1,-rpath,.
}

DESTDIR = build
OBJECTS_DIR = .obj
UI_DIR = .ui
MOC_DIR = .moc
RCC_DIR = .moc

SOURCES += src/main.cpp \
    src/routeBuilder.cpp

HEADERS += \
    src/routeBuilder.h

