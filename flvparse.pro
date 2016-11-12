#-------------------------------------------------
#
# Project created by QtCreator 2016-11-13T00:28:17
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = flvParse
TEMPLATE = app


SOURCES += main.cpp\
        flvparse.cpp\
		FLVStructParse.cpp\
		Util.cpp

HEADERS  += flvparse.h\
		FLVStructParse.h\
		Util.h

FORMS    += flvparse.ui

RESOURCES += \
    flvparse.qrc
