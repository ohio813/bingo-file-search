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
		  core/bingomgr.cpp \
		  ntfs/ntfsdata.cpp \
		  ntfs/volhandlemgr.cpp \
		  ntfs/mft.cpp \
		  ntfs/bingovolinfomgr.cpp \
		  db/masterdb.cpp \
		  db/pathgen.cpp

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
		  core/bingomgr.h \
		  ntfs/ntfsdata.h \
		  ntfs/volhandlemgr.h \
		  ntfs/mft.h \
		  ntfs/bingovolinfomgr.h \
		  db/dbcopy.h \
		  db/masterdb.h \
		  db/pathgen.h