QT = core \
	 sql \
	 network
	 
TEMPLATE = app
TARGET = Demo
DESTDIR = ../../bin
CONFIG += console release
INCLUDEPATH += ../../include
Release:LIBS += ../../lib/Bingo.lib
Debug:LIBS += ../../lib/Bingod.lib

SOURCES = main.cpp
