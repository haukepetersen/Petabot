#-------------------------------------------------
#
# Project created by QtCreator 2013-05-23T10:44:06
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

include(libs/qextserialport/src/qextserialport.pri)

TARGET = FuckupSocketRepeater
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    comm.cpp \
    control.cpp

HEADERS  += mainwindow.h \
    comm.h \
    control.h

FORMS    += mainwindow.ui
