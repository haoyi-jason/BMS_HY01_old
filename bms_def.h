#ifndef BMS_DEF_H
#define BMS_DEF_H

#include <QObject>
#include <QtCore>
#include <QtNetwork>
#include <QDateTime>
//#include "bms_bcudevice.h"

class QTcpSocket;
class CANBUSDevice;
class MODBUSDevice;
class BMS_BCUDevice;
class BMS_System;

namespace bms {
#define GROUP_OF(x)     (x >> 5)
#define ID_OF(x)        (x & 0x1F)
#define GROUP(x)        (x<< 5)
#define ID(x)           (x)
#define SVI_ID          0x1F
#define BCU_ID          0x01
    const int MAX_CELLS = 12;
    const int MAX_NTC = 5;

}

class HW_IOChannel{
public:
    explicit HW_IOChannel(int initValue = 0, bool inverted = false){
        m_value = initValue;
        m_valueToWrite = initValue;
        //m_valueReadBack = initValue;
    }

    int value(){return m_value;}
    void value(int v){m_value = v;}
    void writeValue(int value){m_valueToWrite = value;}
    int writeValue(){return m_valueToWrite;}
    //void setReadback(int value){m_valueReadBack = value;}
    bool valid(){return (m_valueToWrite == m_value);}
    bool valid_write(){return !(m_valueToWrite == m_value);}

private:
    int m_valueToWrite;
    //int m_valueReadBack;
    int m_value;
};

class CAN_Packet{
public:
    quint16 Command;
    QByteArray data;
};

class _EventItem{
public:
    int setValue;
    int resetValue;
    int holdTime;
    int activeTime;
};

class _EventCriteria{
public:
    _EventItem alarmHigh;
    _EventItem alarmLow;
    _EventItem warningHigh;
    _EventItem warningLow;
    bool autoReset = false;
    bool isAlarm();
    bool isWarning();
    int validBand(int v){

    }
    int validHigh(int v){

    }
    int validLow(int v){

    }
};

class _Events{
public:
    int eventType;
    int startTime;
    QString eventDescription;
};



/*
 *  stack information of a BMS
 *  each stack contains n battery and at most 1 stack voltage/current module,
 *  32-devices per stack with 8-bit CANBUS device address mapping, MSB 3-bits
 *  for group, LSB 5-bit for address
 *  1. stack voltage/current module config at fixed address 0x01
 *  2. battery (at most 12-cells) start from 2 to 31, 29 batteries most
 *  3. address 0x0 (group 0/ id 0) is for broadcasting.
 */






class BMS_Event:public QObject{
    Q_OBJECT
public:
    explicit BMS_Event(QObject *parent = nullptr){}

private:
    int m_evtID;
    int m_evtLevel;
    bool m_isAlarm;
    bool m_isWarning;
    QDateTime m_timeStamp;
    QString m_description;

};


class RemoteSystem{
public:
    QString connection="";
    QTcpSocket *socket = nullptr;
    BMS_System *system = nullptr;
    bool configReady = false; // should be false
    QByteArray data;
    QString alias;
    QString logPath;
    int port;
    bool autoConnect;
    void setDigitalOut(int id, int value){
        QString msg = QString("DO:%1:%2").arg(id).arg(value);
        if(socket != nullptr){
            socket->write(msg.toUtf8());
//            socket->flush();
        }
    }

    void setVoltageSource(int id, int value){
        QString msg = QString("VO:%1:%2").arg(id).arg(value);
        if(socket != nullptr){
            socket->write(msg.toUtf8());
//            socket->flush();
        }
    }

    void openSerialPort(QString name, int baudrate, int parity, int stopBits){
        QString msg = QString("PORT:OPEN:%1:%2:%3:%4").arg(name).arg(baudrate).arg(parity).arg(stopBits);
        if(socket != nullptr){
            socket->write(msg.toUtf8());
//            socket->flush();
        }
    }

    void closeSerialPort(QString name){
        QString msg = QString("PORT:CLOSE:%1").arg(name);
        if(socket != nullptr){
            socket->write(msg.toUtf8());
//            socket->flush();
        }
    }

    void writeSerialPort(QString name, QString data){
        QString msg = QString("PORT:WRITE:%1:%2").arg(name).arg(data);
        if(socket != nullptr){
            socket->write(msg.toUtf8());
//            socket->flush();
        }

    }
    void writeCommand(QString command){
        if(socket != nullptr){
            socket->write(command.toUtf8());
//            socket->flush();
        }
    }

    void logData(){
        QByteArray b;
        QDataStream ds(&b,QIODevice::WriteOnly);
        ds << this->system;
        QString path = this->logPath + "/" + QDateTime::currentDateTime().toString("yyyyMMdd_hhmmss.bin");
        QFile f(path);
        if(f.open(QIODevice::WriteOnly)){
            QDataStream df(&f);
            df << b;
            f.close();
        }

    }
};



#endif // BMS_DEF_H
