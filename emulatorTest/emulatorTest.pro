TEMPLATE = lib
CONFIG += console
CONFIG -= app_bundle
CONFIG += qt

QT += core

QMAKE_CXXFLAGS += -std=c++11

TARGET = emtest
DEFINES += EMTEST_LIB

include(../globals.pri)



SOURCES += \
    motorDevice.cpp \
    brickEmulator.cpp \
    motorEmulator.cpp \
    encoderEmulator.cpp

HEADERS += \
    motorDevice.h \
    brickEmulator.h \
    motorEmulator.h \
    encoderEmulator.h

