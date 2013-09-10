#-------------------------------------------------
#
# Project created by QtCreator 2013-06-10T12:40:24
#
#-------------------------------------------------

QT       += core network

QT       -= gui

include(libs/qextserialport/src/qextserialport.pri)

TARGET = FuckupHCU
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp \
    fuckuphcu.cpp \
    command.cpp \
    uartsocket.cpp \
    tcpsocket.cpp \
    commandparser.cpp

HEADERS += \
    fuckuphcu.h \
    command.h \
    comm.h \
    uartsocket.h \
    tcpsocket.h \
    commandparser.h
