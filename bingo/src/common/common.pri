QT = core \
	 sql \
	 network

DEFINES += BINGO_NAMESPACE=Bingo

INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD

SOURCES = core/threadsync.cpp \
		  core/multiprocessorthread.cpp \
		  core/log.cpp \
		  core/memory.cpp \
		  core/globaldata.cpp \
		  core/bingotime.cpp \
		  core/bingosize.cpp \
		  ntfs/ntfsdata.cpp

HEADERS = core/global.h \
          core/threadsync.h \
		  core/multiprocessorthread.h \
		  core/log.h \
		  core/memory.h \
		  core/globaldata.h \
		  core/stringconvert.h \
		  core/lrucache.hpp\
		  core/blockingqueue.hpp \
		  core/bingotime.h \
		  core/bingosize.h \
		  ntfs/ntfsdata.h