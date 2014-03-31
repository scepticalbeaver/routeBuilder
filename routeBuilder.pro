TRIKCONTROL_BINDIR = $$PWD/trikRuntime/bin/arm-release
TRIKCONTROL_DIR = $$PWD/trikRuntime/trikControl

QT       += core
QT       += network
QT       += gui

TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG += qt
QMAKE_CXXFLAGS += -std=c++11


TARGET = routeBuilder

INCLUDEPATH = \
	$$PWD \
	$$BOOST_DIR \
	$$TRIKCONTROL_DIR/include \

LIBS += -L$$TRIKCONTROL_BINDIR -ltrikControl

!macx {
	QMAKE_LFLAGS += -Wl,-O1,-rpath,.
}


DESTDIR = build
OBJECTS_DIR = .obj
UI_DIR = .ui
MOC_DIR = .moc
RCC_DIR = .moc

SOURCES += src/main.cpp \
    src/routeBuilder.cpp \
    src/trackStorage.cpp

HEADERS += \
    src/routeBuilder.h \
    src/trackStorage.h

