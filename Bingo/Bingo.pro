#-------------------------------------------------
#
# Project created by QtCreator 2011-12-26T14:07:35
#
#-------------------------------------------------

QT       += core gui

TARGET = Bingo
TEMPLATE = app


SOURCES += main.cpp\
    ui/mainwindow.cpp \
    core/VolumeMgr.cpp \
    core/USN.cpp \
    core/ThreadSync.cpp \
    core/NTFS.cpp \
    core/Memory.cpp \
    core/File.cpp \
    core/Data.cpp \
    util/StringConvert.cpp \
    util/Log.cpp \
    util/Ini.cpp

HEADERS  += ui/mainwindow.h \
    core/VolumeMgr.h \
    core/VolumeLetter.h \
    core/USN.h \
    core/ThreadSync.h \
    core/NTFS.h \
    core/Memory.h \
    core/File.h \
    core/Data.h \
    util/StringConvert.h \
    util/LRUCache.hpp \
    util/Log.h \
    util/Ini.h \

FORMS    += ui/mainwindow.ui

RESOURCES += \
    ui/resource.qrc











