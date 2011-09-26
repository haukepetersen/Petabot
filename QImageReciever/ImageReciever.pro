#-------------------------------------------------
#
# Project created by QtCreator 2011-07-20T22:37:00
#
#-------------------------------------------------

QT       += core gui

TARGET = ImageReciever
TEMPLATE = app

SOURCES += main.cpp\
        mainwindow.cpp \
    imagereciever.cpp

HEADERS  += mainwindow.h \
    imagereciever.h

FORMS    += mainwindow.ui

LIBS += -L/usr/lib/ -lcv -lhighgui
