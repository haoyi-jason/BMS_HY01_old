#include "bms_localconfig.h"
#include <QFile>
#include<QJsonDocument>
#include<QJsonObject>
#include<QJsonValue>
#include<QJsonArray>

BMS_LocalConfig::BMS_LocalConfig(QObject *parent) : QObject(parent)
{

}

void BMS_LocalConfig::load(QByteArray b)
{
    QJsonParseError e;
    QJsonDocument d = QJsonDocument::fromJson(b,&e);
    if(d.isNull()) return;

    QJsonObject obj = d.object();

    if(obj.contains("system")){
        QJsonObject ob = obj["system"].toObject();
        system.parseJson(ob);
    }
    if(obj.contains("balancing")){
        QJsonObject ob = obj["balancing"].toObject();
        balancing.parseJson(ob);
    }
    if(obj.contains("stack")){
        QJsonObject ob = obj["stack"].toObject();
        stack.parseJson(ob);
    }
    if(obj.contains("criteria")){
        QJsonObject ob = obj["criteria"].toObject();
        criteria.parseJson(ob);
    }
    if(obj.contains("log")){
        QJsonObject ob = obj["log"].toObject();
        record.parseJson(ob);
    }
    if(obj.contains("modbus")){
        QJsonObject ob = obj["modbus"].toObject();
        modbus.parseJson(ob);
    }
    if(obj.contains("network")){
        QJsonObject ob = obj["network"].toObject();
        network.parseJson(ob);
    }
}

void BMS_LocalConfig::load(QString fileName)
{
    QFile f(fileName);
    if(f.open(QIODevice::ReadOnly)){
        load(f.readAll());
        f.close();
    }

}

void BMS_LocalConfig::save(QString fileName)
{
    QFile f(fileName);
    if(f.open(QIODevice::ReadOnly)){
        QJsonParseError e;
        QJsonDocument d = QJsonDocument::fromJson(f.readAll(),&e);
        f.close();
        if(d.isNull()) return ;
        QJsonObject obj = d.object();
        QJsonObject *ob;
        ob = new QJsonObject;
        system.feedJson(ob);
        obj["system"] = *ob;

        ob = new QJsonObject;
        balancing.feedJson(ob);
        obj["balancing"] = *ob;

        ob = new QJsonObject;
        stack.feedJson(ob);
        obj["stack"] = *ob;

        ob = new QJsonObject;
        criteria.feedJson(ob);
        obj["criteria"] = *ob;

        ob = new QJsonObject;
        record.feedJson(ob);
        obj["log"] = *ob;

        ob = new QJsonObject;
        modbus.feedJson(ob);
        obj["modbus"] = *ob;

        ob = new QJsonObject;
        network.feedJson(ob);
        obj["network"] = *ob;

        d.setObject(obj);
        f.open(QIODevice::WriteOnly);
        f.write(d.toJson());
        f.close();

    }
}

void BMS_LocalConfig::genDefault(QString fileName)
{
    balancing.BalancingVolt = "3.4";
    balancing.HystersisMV = "8";
    balancing.On_TimeSec = balancing.Off_TimeSec = "30";

    stack.Capacity = "50";
    stack.BatteryPerStack = "3";
    stack.StackCount = "1";
    stack.CellPerBattery = "12";
    stack.NTCPerBattery = "5";

    criteria.cell.volt_warning.High_Set = "3.7";
    criteria.cell.volt_warning.High_Clr = "3.6";
    criteria.cell.volt_warning.Low_Set = "3.0";
    criteria.cell.volt_warning.Low_Clr = "3.2";

    criteria.cell.volt_alarm.High_Set = "3.7";
    criteria.cell.volt_alarm.High_Clr = "3.6";
    criteria.cell.volt_alarm.Low_Set = "3.0";
    criteria.cell.volt_alarm.Low_Clr = "3.2";

    criteria.cell.temp_warning.High_Set = "45";
    criteria.cell.temp_warning.High_Clr = "40";
    criteria.cell.temp_warning.Low_Set = "10";
    criteria.cell.temp_warning.Low_Clr = "20";

    criteria.cell.temp_alarm.High_Set = "45";
    criteria.cell.temp_alarm.High_Clr = "40";
    criteria.cell.temp_alarm.Low_Set = "10";
    criteria.cell.temp_alarm.Low_Clr = "20";

    criteria.stack.volt_warning.High_Set = "130";
    criteria.stack.volt_warning.High_Clr = "128";
    criteria.stack.volt_warning.Low_Set = "80";
    criteria.stack.volt_warning.Low_Clr = "90";

    criteria.stack.volt_alarm.High_Set = "130";
    criteria.stack.volt_alarm.High_Clr = "128";
    criteria.stack.volt_alarm.Low_Set = "80";
    criteria.stack.volt_alarm.Low_Clr = "90";

    criteria.stack.temp_warning.High_Set = "0";
    criteria.stack.temp_warning.High_Clr = "0";
    criteria.stack.temp_warning.Low_Set = "0";
    criteria.stack.temp_warning.Low_Clr = "0";

    criteria.stack.temp_alarm.High_Set = "0";
    criteria.stack.temp_alarm.High_Clr = "0";
    criteria.stack.temp_alarm.Low_Set = "0";
    criteria.stack.temp_alarm.Low_Clr = "0";

    criteria.soc.warning.High_Set = "0";
    criteria.soc.warning.High_Clr = "0";
    criteria.soc.warning.Low_Set = "15";
    criteria.soc.warning.Low_Clr = "20";

    criteria.soc.alarm.High_Set = "0";
    criteria.soc.alarm.High_Clr = "0";
    criteria.soc.alarm.Low_Set = "15";
    criteria.soc.alarm.Low_Clr = "20";

}
