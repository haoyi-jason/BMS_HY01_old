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
    if(obj.contains("event_output")){
        QJsonObject ob = obj["event_output"].toObject();
        event_output.parseJson(ob);
    }
    if(obj.contains("canbus")){
        QJsonObject ob = obj["canbus"].toObject();
        conbus.parseJson(ob);
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

        ob = new QJsonObject;
        event_output.feedJson(ob);
        obj["event_output"] = *ob;

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

BMS_AlarmModel::BMS_AlarmModel(BMS_LocalConfig *cfg, QObject *parent):
    QAbstractTableModel(parent)
{
    m_header << "Item"<<"Enable?";
    m_header << "WSET LOW" << "WCLR LOW" << "WSET HIGH" << "WCLR HIGH";
    m_header << "ASET LOW" << "ACLR LOW" << "ASET HIGH" << "ACLR HIGH";
    m_header << "Duration";

    m_rowHeader << "CELL Voltage" << "CELL Temp" << "STACK Volt" << "Stack Temp";
    m_rowHeader << "SOC";

    m_config = cfg;
}

BMS_AlarmModel::~BMS_AlarmModel()
{

}

int BMS_AlarmModel::rowCount(const QModelIndex &parent) const{
    return m_rowHeader.count();
}
int BMS_AlarmModel::columnCount(const QModelIndex &parent) const
{
    return m_header.count();
}
QVariant BMS_AlarmModel::data(const QModelIndex &index, int role) const
{
    int r = index.row();
    int c = index.column();
    BMS_SystemCriteriaConfig cc = m_config->criteria;
    BMS_CriteriaConfig cell = cc.cell;
    BMS_CriteriaConfig stack= cc.stack;
    BMS_CriteriaSOC soc = cc.soc;
    switch(role){
    case Qt::DisplayRole:
    case Qt::EditRole:
        switch(r){
        case 0:
            switch(c){
            case 0: return QVariant(cell.EnableV);break;
            case 1: return QVariant(cell.volt_warning.Low_Set);break;
            case 2: return QVariant(cell.volt_warning.Low_Clr);break;
            case 3: return QVariant(cell.volt_warning.High_Set);break;
            case 4: return QVariant(cell.volt_warning.High_Clr);break;
            case 5: return QVariant(cell.volt_alarm.Low_Set);break;
            case 6: return QVariant(cell.volt_alarm.Low_Clr);break;
            case 7: return QVariant(cell.volt_alarm.High_Set);break;
            case 8: return QVariant(cell.volt_alarm.High_Clr);break;
            default: return QVariant();break;
            }
            break;
        case 1:
            switch(c){
            case 0: return QVariant(cell.EnableT);break;
            case 1: return QVariant(cell.temp_warning.Low_Set);break;
            case 2: return QVariant(cell.temp_warning.Low_Clr);break;
            case 3: return QVariant(cell.temp_warning.High_Set);break;
            case 4: return QVariant(cell.temp_warning.High_Clr);break;
            case 5: return QVariant(cell.temp_alarm.Low_Set);break;
            case 6: return QVariant(cell.temp_alarm.Low_Clr);break;
            case 7: return QVariant(cell.temp_alarm.High_Set);break;
            case 8: return QVariant(cell.temp_alarm.High_Clr);break;
            default: return QVariant();break;
            }
            break;
        case 2:
            switch(c){
            case 0: return QVariant(stack.EnableV);break;
            case 1: return QVariant(stack.volt_warning.Low_Set);break;
            case 2: return QVariant(stack.volt_warning.Low_Clr);break;
            case 3: return QVariant(stack.volt_warning.High_Set);break;
            case 4: return QVariant(stack.volt_warning.High_Clr);break;
            case 5: return QVariant(stack.volt_alarm.Low_Set);break;
            case 6: return QVariant(stack.volt_alarm.Low_Clr);break;
            case 7: return QVariant(stack.volt_alarm.High_Set);break;
            case 8: return QVariant(stack.volt_alarm.High_Clr);break;
            default: return QVariant();break;
            }
            break;
        case 3:
            switch(c){
            case 0: return QVariant(stack.EnableT);break;
            case 1: return QVariant(stack.temp_warning.Low_Set);break;
            case 2: return QVariant(stack.temp_warning.Low_Clr);break;
            case 3: return QVariant(stack.temp_warning.High_Set);break;
            case 4: return QVariant(stack.temp_warning.High_Clr);break;
            case 5: return QVariant(stack.temp_alarm.Low_Set);break;
            case 6: return QVariant(stack.temp_alarm.Low_Clr);break;
            case 7: return QVariant(stack.temp_alarm.High_Set);break;
            case 8: return QVariant(stack.temp_alarm.High_Clr);break;
            default: return QVariant();break;
            }
            break;
        case 4:
            switch(c){
            case 0: return QVariant(soc.EnableV);break;
            case 1: return QVariant(soc.warning.Low_Set);break;
            case 2: return QVariant(soc.warning.Low_Clr);break;
            case 3: return QVariant(soc.warning.High_Set);break;
            case 4: return QVariant(soc.warning.High_Clr);break;
            case 5: return QVariant(soc.alarm.Low_Set);break;
            case 6: return QVariant(soc.alarm.Low_Clr);break;
            case 7: return QVariant(soc.alarm.High_Set);break;
            case 8: return QVariant(soc.alarm.High_Clr);break;
            default: return QVariant();break;
            }
            break;
        default:return QVariant();break;
        }

        break;
    case Qt::TextAlignmentRole:
        return (Qt::AlignRight);
    case Qt::CheckStateRole:
        return Qt::Checked;
        break;
    default:break;
    }
    return QVariant();
}

bool BMS_AlarmModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    return false;
}

QVariant BMS_AlarmModel::headerData(int section,Qt::Orientation orientation, int role) const
{
    if(role == Qt::DisplayRole){
        if(orientation == Qt::Horizontal){
            return QVariant(m_header[section]);
        }
        else if(orientation == Qt::Vertical){
            return QVariant(m_rowHeader[section]);
        }
    }
    return QVariant();
}
Qt::ItemFlags BMS_AlarmModel::flags(const QModelIndex &index) const
{
    if(index.column() == 0){
        return Qt::ItemIsUserCheckable | QAbstractTableModel::flags(index);
    }
    return Qt::ItemIsEditable | QAbstractTableModel::flags(index);
}


