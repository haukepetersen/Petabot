#-------------------------------------------------
#
# Project created by QtCreator 2015-04-30T21:17:20
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = petactrl
TEMPLATE = app


SOURCES += main.cpp\
        peta.cpp \
    joydev.cpp

HEADERS  += peta.h \
    joydev.h

FORMS    += peta.ui

win32{
    INCLUDEPATH += "D:\Programme\SDL\SDL-1.2.15\include"
    LIBS += -L"D:\Programme\SDL\SDL-1.2.15\lib"
}
unix{
    INCLUDEPATH += "/usr/local/qwt-6.1.0/include"
}

LIBS	  += -lSDL
