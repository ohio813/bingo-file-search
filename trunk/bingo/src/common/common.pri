QT = core \
	 sql \
	 network

DEFINES += BINGO_NAMESPACE=Bingo

INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD

SOURCES = core/threadsync.cpp \
		  core/multiprocessorthread.cpp \
		  core/memory.cpp \
		  core/globaldata.cpp

HEADERS = core/global.h \
          core/threadsync.h \
		  core/multiprocessorthread.h \
		  core/log.hpp \
		  core/memory.h \
		  core/globaldata.h \
		  core/stringconvert.h \
		  core/lrucache.hpp\
		  core/blockingqueue.hpp