#-------------------------------------------------
#
# Project created by QtCreator 2013-05-28T10:25:40
#
#-------------------------------------------------

QT       += core gui network
CONFIG   += qwt

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

include(libs/qextserialport/src/qextserialport.pri)

TARGET = FuckupCtrl
TEMPLATE = app

SOURCES += main.cpp\
        fuckupctrl.cpp \
    datasink.cpp \
    inputconfig.cpp \
    command.cpp \
    commandparser.cpp \
    uartsocket.cpp \
    tcpsocket.cpp \
    testcommander.cpp \
    joydev.cpp

HEADERS  += fuckupctrl.h \
    datasink.h \
    inputconfig.h \
    comm.h \
    command.h \
    commandparser.h \
    uartsocket.h \
    tcpsocket.h \
    testcommander.h \
    joydev.h

FORMS    += fuckupctrl.ui \
    datasink.ui \
    inputconfig.ui \
    testcommander.ui

RESOURCES += \
    resources.qrc

win32{
    INCLUDEPATH += "D:\Programme\SDL\SDL-1.2.15\include"
    LIBS += -L"D:\Programme\SDL\SDL-1.2.15\lib"
}
unix{
    INCLUDEPATH += "/usr/local/qwt-6.1.0/include"
    LIBS += -L"/usr/local/qwt-6.1.0/lib" -lqwt
}

LIBS	  += -lSDL
