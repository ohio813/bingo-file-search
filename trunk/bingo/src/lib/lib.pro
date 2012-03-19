include(../common/common.pri)

TEMPLATE = lib
TARGET = Bingo
DESTDIR = ../../lib
DLLDESTDIR = ../../bin
CONFIG(debug,debug|release) { 
	TARGET = $$join(TARGET,,,d)
}

DEFINES += BINGO_SHARED

SOURCES += 

HEADERS += bingo.h