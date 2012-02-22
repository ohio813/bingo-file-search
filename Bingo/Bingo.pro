#-------------------------------------------------
#
# Project created by QtCreator 2011-12-26T14:07:35
#
#-------------------------------------------------

QT       += core\
		gui \
		sql \
		network

TARGET = Bingo
TEMPLATE = app


SOURCES += main.cpp\
    util/StringConvert.cpp \
    util/Log.cpp \
    util/Ini.cpp \
    core/VolumeMgr.cpp \
    core/USN.cpp \
    core/ThreadSync.cpp \
    core/NTFS.cpp \
    core/Moniter.cpp \
    core/Memory.cpp \
    core/File.cpp \
    core/Data.cpp \
    core/CoreMgr.cpp \
    ui/settingwidget.cpp \
    ui/searchwidget.cpp \
    ui/mainwindow.cpp \
    ui/Language.cpp \
	ui/loadingwidget.cpp \
	ui/waitingwidget.cpp \
	ui/DispModel.cpp \
	ui/dispview.cpp \
    util/StringConvert.cpp \
    util/PathDB.cpp \
    util/MasterDB.cpp \
    util/Log.cpp \
    util/Ini.cpp \
    util/ConfigDB.cpp

HEADERS  += util/StringConvert.h \
    util/LRUCache.hpp \
    util/Log.h \
    util/Ini.h \
    core/VolumeMgr.h \
    core/VolumeLetter.h \
    core/USN.h \
    core/ThreadSync.h \
    core/NTFS.h \
    core/Moniter.h \
    core/Memory.h \
    core/File.h \
    core/Data.h \
    core/CoreMgr.h \
    ui/settingwidget.h \
    ui/searchwidget.h \
    ui/mainwindow.h \
    ui/Language.h \
    ui/IconCache.hpp \
	ui/loadingwidget.h \
	ui/DispModel.h \
	ui/dispview.h \
	ui/waitingwidget.h \
    util/StringConvert.h \
    util/PathDB.h \
    util/MasterDB.h \
    util/LRUCache.hpp \
    util/Log.h \
    util/Ini.h \
    util/ConfigDB.h \
    util/BlockingQueue.hpp

FORMS    += ui/mainwindow.ui \
    ui/settingwidget.ui \
    ui/searchwidget.ui \
	ui/loadingwidget.ui \
	ui/waitingwidget.ui

RESOURCES += \
    ui/resource.qrc



















