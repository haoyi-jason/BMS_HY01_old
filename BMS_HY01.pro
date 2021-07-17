#-------------------------------------------------
#
# Project created by QtCreator 2021-02-18T20:19:14
#
#-------------------------------------------------

QT       += core gui serialport network serialbus charts

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
        bms_bcudevice.cpp \
        bms_bmudevice.cpp \
        bms_def.cpp \
        bms_localconfig.cpp \
        bms_model.cpp \
        bms_stack.cpp \
        bms_svidevice.cpp \
        bms_system.cpp \
        bmscollector.cpp \
        can_codec.cpp \
        collectorview.cpp \
        frmeventview.cpp \
        frmhardwareconfig.cpp \
        frmhistoryview.cpp \
        frmhwconfig.cpp \
        frmstackconfig.cpp \
        frmstackview.cpp \
        frmtest.cpp \
        historychart.cpp \
        inputwin.cpp \
        loginvalid.cpp \
        main.cpp \
        maininfo.cpp \
        mainwindow.cpp \
        secs.cpp \
        stackinfo.cpp

HEADERS += \
        bms_bcudevice.h \
        bms_bmudevice.h \
        bms_def.h \
        bms_localconfig.h \
        bms_model.h \
        bms_stack.h \
        bms_svidevice.h \
        bms_system.h \
        bmscollector.h \
        can_codec.h \
        collectorview.h \
        frmeventview.h \
        frmhardwareconfig.h \
        frmhistoryview.h \
        frmhwconfig.h \
        frmstackconfig.h \
        frmstackview.h \
        frmtest.h \
        historycharg.h \
        historychart.h \
        inputwin.h \
        loginvalid.h \
        maininfo.h \
        mainwindow.h \
        secs.h \
        stackinfo.h

FORMS += \
        collectorview.ui \
        frmeventview.ui \
        frmhardwareconfig.ui \
        frmhistoryview.ui \
        frmhwconfig.ui \
        frmstackconfig.ui \
        frmstackview.ui \
        frmtest.ui \
        loginvalid.ui \
        maininfo.ui \
        mainwindow.ui \
        stackinfo.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

INCLUDEPATH += "icpdas_usbcan"

#LIBS += VCI_CAN.lib

RESOURCES += \
    resource.qrc

