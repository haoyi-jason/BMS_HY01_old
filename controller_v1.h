#ifndef CONTROLLER_V1_H
#define CONTROLLER_V1_H

#include <QObject>
#include <QCanBus>
#include <QCanBusDevice>
#include <QCanBusDeviceInfo>
#include "bms_def.h"

class QTcpSocket;
class QTcpServer;
class BMS_System;
class QTimer;
class RemoteSystem;
class QSerialPort;
class QModbusRtuSerialSlave;
class BMS_StateMachine;
class QModbusServer;
class QUdpSocket;

class CANBUSDevice{
public:
    QString name="";
    bool connected=false;
    QList<int> groupList;
    bool error=false;
    QString errorString="";
    int bitrate;
    QCanBusDevice *dev = nullptr;
    QCanBusDeviceInfo *info = nullptr;
};

class MODBUSDevice{
public:
    QString portName;
    int bitrate;
    bool connected=false;
    QModbusServer *dev;
    int tcpPort;
};

class Controller_V1
{
public:
    Controller_V1();
    void LoadConfig(QString defaultPath="");

};


class StateMachine_V1 : public QObject
{
    Q_OBJECT
public:
    enum BMS_Controller_State{
        STATE_NONE,
        STATE_NOT_INITIALIZED,
        STATE_INITIALIZING,
        STATE_INITIALIZED,
        STATE_IDLE,
        STATE_NORMAL,
        STATE_WRITE_FRAME,
        STATE_WAIT_RESP,
        STATE_TERMINATE,
        STATE_TERMINATED,
    };
    Q_ENUM(BMS_Controller_State)

    explicit BMS_StateMachine(QObject *parent = nullptr);

    void setState(BMS_Controller_State state);
    bool isWaitResp();
    void feedFrame(QCanBusFrame f);
    void add_packet(CAN_Packet *p);
    void add_emg_packet(CAN_Packet *p);
    //BMS_Controller_State state();
    CAN_Packet *popPacket();

signals:
    void message(const QString &message);

public slots:

public:
    BMS_Controller_State state = STATE_NONE;
    BMS_Controller_State pendState = STATE_NONE;
    int subState=0;
    QList<CAN_Packet*> m_pendPacket;
    CAN_Packet *m_currPacket = nullptr;
    int stateDelay = 0;
    int stateRetry = 0;
    int vsource_delay = 0;
};

#endif // CONTROLLER_V1_H
