#-------------------------------------------------
#
# Project created by QtCreator 2013-06-13T22:15:09
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Petabot
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    imagereceiver.cpp

HEADERS  += mainwindow.h \
    imagereceiver.h

FORMS    += mainwindow.ui

win32 {
LIBS += -lwsock32
}
