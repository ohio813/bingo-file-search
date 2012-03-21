QT = core \
	 gui \
	 sql \
	 network
	 
TEMPLATE = app
TARGET = Demo
DESTDIR = ../../bin
CONFIG += console release
INCLUDEPATH += ../../include
Release:LIBS += ../../lib/Bingo.lib
Debug:LIBS += ../../lib/Bingod.lib
DEFINES += LOG_OUTPUT_TO_SCREEN

SOURCES = main.cpp
