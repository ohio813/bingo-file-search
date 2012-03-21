include(../common/common.pri)

QT += gui
TEMPLATE = app
TARGET = BingoServer
DESTDIR = ../../bin
CONFIG += console release
DEFINES += LOG_OUTPUT_TO_SCREEN

SOURCES += main.cpp

HEADERS += 