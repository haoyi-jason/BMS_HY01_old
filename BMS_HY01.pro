#-------------------------------------------------
#
# Project created by QtCreator 2021-02-18T20:19:14
#
#-------------------------------------------------

QT       += core gui serialport network serialbus

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = BMS_HY01
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++11

SOURCES += \
        bms_def.cpp \
        bms_model.cpp \
        bmscollector.cpp \
        can_codec.cpp \
        frmeventview.cpp \
        frmhardwareconfig.cpp \
        frmhistoryview.cpp \
        frmstackconfig.cpp \
        main.cpp \
        maininfo.cpp \
        mainwindow.cpp \
        secs.cpp \
        stackinfo.cpp

HEADERS += \
        bms_def.h \
        bms_model.h \
        bmscollector.h \
        can_codec.h \
        frmeventview.h \
        frmhardwareconfig.h \
        frmhistoryview.h \
        frmstackconfig.h \
        maininfo.h \
        mainwindow.h \
        secs.h \
        stackinfo.h

FORMS += \
        frmeventview.ui \
        frmhardwareconfig.ui \
        frmhistoryview.ui \
        frmstackconfig.ui \
        maininfo.ui \
        mainwindow.ui \
        stackinfo.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

INCLUDEPATH += "icpdas_usbcan"

#LIBS += VCI_CAN.lib

