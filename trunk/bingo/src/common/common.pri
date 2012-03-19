QT = core \
	 sql \
	 network

DEFINES += BINGO_NAMESPACE=Bingo

INCLUDEPATH += ../../src/common
DEPENDPATH += ../../src/common

SOURCES = core/threadsync.cpp \
		  core/multiprocessorthread.cpp

HEADERS = core/global.h \
          core/threadsync.h \
		  core/multiprocessorthread.h