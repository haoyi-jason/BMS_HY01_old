#include "bms_localconfig.h"
#include <QFile>
#include<QJsonDocument>
#include<QJsonObject>
#include<QJsonValue>
#include<QJsonArray>

BMS_LocalConfig::BMS_LocalConfig(QObject *parent) : QObject(parent)
{

}

void BMS_LocalConfig::load(QString fileName)
{
    QFile f(fileName);
    if(f.open(QIODevice::ReadOnly)){
        QJsonParseError e;
        QJsonDocument d = QJsonDocument::fromJson(f.readAll(),&e);
        f.close();
        if(d.isNull()) return;

        QJsonObject obj = d.object();

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
        if(obj.contains("enable_log")){
            EnableLog = obj["enable_log"].toBool();
        }
        if(obj.contains("log_days")){
            LogDays = obj["log_days"].toString();
        }
        if(obj.contains("log_records")){
            LogRecords = obj["log_records"].toString();
        }
        if(obj.contains("simulate")){
            Simulate = obj["simulate"].toBool();
        }
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
        //if(obj.contains("balancing")){
            //ob = obj["balancing"].toObject();
            ob = new QJsonObject;
            balancing.feedJson(ob);
            obj["balancing"] = *ob;
        //}
        //if(obj.contains("stack")){
            //ob = obj["stack"].toObject();
            ob = new QJsonObject;
            stack.feedJson(ob);
            obj["stack"] = *ob;
        //}
        //if(obj.contains("criteria")){
//            ob = obj["criteria"].toObject();
            ob = new QJsonObject;
            criteria.feedJson(ob);
            obj["criteria"] = *ob;
        //}
        //if(obj.contains("enable_log")){
            obj["enable_log"] = EnableLog;
        //}
        //if(obj.contains("log_days")){
            obj["log_days"] = LogDays;
        //}
        //if(obj.contains("log_records")){
            obj["log_records"] = LogRecords;
        //}
        //if(obj.contains("simulate")){
            obj["simulate"] = Simulate;
        //}
            ob = new QJsonObject;
            modbus.feedJson(ob);
            obj["modbus"] = *ob;

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
