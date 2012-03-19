CONFIG += ordered qt
TARGET = Bingo
DESTDIR = bin
TEMPLATE = subdirs

SUBDIRS = src/lib \
          src/libdemo \
          src/server \
		  src/app 

RESOURCES = resource
