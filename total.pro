TEMPLATE = subdirs
SUBDIRS = \
    routeBuilder \
    emulatorTest

routeBuilder.file = routeBuilder/routeBuilder.pro
emtest.file = emulatorTest/emulatorTest.pro
emtest.depends = routeBuilder


