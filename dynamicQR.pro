#-------------------------------------------------
#
# Project created by QtCreator 2018-03-27T20:39:29
#
#-------------------------------------------------


QT       += core gui
QT       += multimedia

LIBS += -lpthread libwsock32 libws2_32

CONFIG += c++11

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = dynamicQR2
TEMPLATE = app



LIBS += D:\workspace\QT\SDK\JHCap2.lib\
#    D:\workspace\QT\SDK\vld.lib\
#    D:\workspace\QT\myGraduation\dynamicQR\MVCAMSDK.dll

INCLUDEPATH += D:\workspace\QT\SDK\
        D:\opencv249\opencv2410\build\x86\mingw\OpenCV\include\opencv2\
        D:\opencv249\opencv2410\build\x86\mingw\OpenCV\include\opencv\
        D:\opencv249\opencv2410\build\x86\mingw\OpenCV\include\


SOURCES += main.cpp\
        mainwindow.cpp \
    capturethread.cpp \
    Blurlen_detect.cpp \
    QRcode_recovery.cpp \
    QRcodeCuter.cpp \
    Wiener_filter.cpp \
    basicOperation.cpp

HEADERS  += mainwindow.h \
    capturethread.h \
    capturethread.h \
    Blurlen_detect.h \
    QRcode_recovery.h \
    QRcodeCuter.h \
    Wiener_filter.h \
    basicoperation.h

#include($$PWD\QZXing\QZXing.pri)
include (D:\workspace\QT\qzxing-master\src/QZXing.pri)

FORMS    += \
    mainwindow.ui


LIBS +=D:\opencv249\opencv2410\build\x86\mingw\OpenCV\lib\libopencv_calib3d244.dll.a\
    D:\opencv249\opencv2410\build\x86\mingw\OpenCV\lib\libopencv_contrib244.dll.a\
D:\opencv249\opencv2410\build\x86\mingw\OpenCV\lib\libopencv_core244.dll.a\
D:\opencv249\opencv2410\build\x86\mingw\OpenCV\lib\libopencv_features2d244.dll.a\
D:\opencv249\opencv2410\build\x86\mingw\OpenCV\lib\libopencv_flann244.dll.a\
D:\opencv249\opencv2410\build\x86\mingw\OpenCV\lib\libopencv_gpu244.dll.a\
D:\opencv249\opencv2410\build\x86\mingw\OpenCV\lib\libopencv_highgui244.dll.a\
D:\opencv249\opencv2410\build\x86\mingw\OpenCV\lib\libopencv_imgproc244.dll.a\
D:\opencv249\opencv2410\build\x86\mingw\OpenCV\lib\libopencv_legacy244.dll.a\
D:\opencv249\opencv2410\build\x86\mingw\OpenCV\lib\libopencv_ml244.dll.a\
D:\opencv249\opencv2410\build\x86\mingw\OpenCV\lib\libopencv_nonfree244.dll.a\
D:\opencv249\opencv2410\build\x86\mingw\OpenCV\lib\libopencv_objdetect244.dll.a\
D:\opencv249\opencv2410\build\x86\mingw\OpenCV\lib\libopencv_photo244.dll.a\
D:\opencv249\opencv2410\build\x86\mingw\OpenCV\lib\libopencv_stitching244.dll.a\
D:\opencv249\opencv2410\build\x86\mingw\OpenCV\lib\libopencv_ts244.a\
D:\opencv249\opencv2410\build\x86\mingw\OpenCV\lib\libopencv_video244.dll.a\
D:\opencv249\opencv2410\build\x86\mingw\OpenCV\lib\libopencv_videostab244.dll.a
