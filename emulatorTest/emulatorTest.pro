TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG += qt

QT += core

QMAKE_CXXFLAGS += -std=c++11

TARGET = emtest

DESTDIR = bin
OBJECTS_DIR = .obj
MOC_DIR = .moc

SOURCES += main.cpp \
    motorDevice.cpp \
    brickEmulator.cpp

HEADERS += \
    motorDevice.h \
    brickEmulator.h

