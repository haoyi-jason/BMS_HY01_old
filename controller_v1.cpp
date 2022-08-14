#include "controller_v1.h"
#include <QtNetwork>
#include <QtCore>
#include <QCanBus>
#include <QCanBusDevice>
#include <QCanBusDeviceInfo>
#include <QCanBusFrame>
#include <QProcess>
#include <QDateTime>
#include <QSysInfo>
#include<QJsonDocument>
#include<QJsonObject>
#include<QJsonValue>
#include<QJsonArray>
#include <QtSerialPort/QSerialPort>
#include <QModbusRtuSerialSlave>
#include <QModbusTcpServer>
#include "bms_def.h"
#include "bms_bmudevice.h"
#include "bms_bcudevice.h"
#include "bms_svidevice.h"
#include "bms_stack.h"
#include "bms_system.h"



Controller_V1::Controller_V1()
{
}

void Controller_V1::LoadConfig(QString defaultPath)
{

}
