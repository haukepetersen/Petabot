#-------------------------------------------------
#
# Project created by QtCreator 2013-06-13T22:48:44
#
#-------------------------------------------------

QT       += core network

QT       -= gui

TARGET = ImageProcessor
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

HEADERS += \
    imageprocessor.h

SOURCES += imageprocessor.cpp \
    main.cpp

LIBS    += -llibopencv_core245 -llibopencv_imgproc245 -llibopencv_highgui245 -llibopencv_ml245 -llibopencv_video245 -llibopencv_features2d245 -llibopencv_calib3d245 -llibopencv_objdetect245 -llibopencv_contrib245 -llibopencv_legacy245 -llibopencv_flann245

win32 {
LIBS += -LD:\opencv\lib

INCLUDEPATH += D:\opencv\include
}
