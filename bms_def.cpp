#include <QObject>
#include<QJsonDocument>
#include<QJsonObject>
#include<QJsonValue>
#include<QJsonArray>
#include "bms_model.h"

/***** BMS_BatteryInfo ************/
BMS_BMUDevice::BMS_BMUDevice(QObject *parent):
    QObject(parent)
{

    for(int i=0;i<bms::MAX_CELLS;i++){
        m_cellVoltage.append(0);
        m_balancing.append(0);
        m_openWire.append(0);
    }
    for(int i=0;i<bms::MAX_NTC;i++){
        m_packTemperature.append(0);
    }
}
BMS_BMUDevice::BMS_BMUDevice(int nofCells, int nofTemp, QObject *parent):
    QObject(parent),m_nofCell(nofCells), m_nofNtc(nofTemp)
{
    for(int i=0;i<nofCells;i++){
        m_cellVoltage.append(0);
        m_balancing.append(0);
    }
    for(int i=0;i<nofTemp;i++){
        m_packTemperature.append(0);
    }
}

ushort BMS_BMUDevice::getCellVoltage(int index){
    if(index < m_cellVoltage.size()){
        return m_cellVoltage.at(index);
    }
    return 0;
}

ushort BMS_BMUDevice::getPackTemperature(int index)
{
    if(index < m_packTemperature.size()){
        return m_packTemperature.at((index));
    }
    return 0;
}

/***** BMS_StackInfo **************/
BMS_StackInfo::BMS_StackInfo():
    m_soc(0),m_soh(0),m_groupID(0),m_State("Idle"),m_StackVoltage(0),
    m_StackCurrent(0),m_MaxCellIndex(-1),m_MinCellIndex(-1),
    m_MaxTemperature(0),m_MinTemperature(0)
{
}


int BMS_StackInfo::BatteryCount()
{
    return m_batteries.size();
}

ushort BMS_StackInfo::cellVoltage(int bid, int cid)
{
    if(bid < m_batteries.size()){
        BMS_BMUDevice *info = m_batteries.at(bid);
        if(cid < 12){
            return info->getCellVoltage(cid);
        }
    }
    return 0;
}

void BMS_StackInfo::cellVoltage(int bid, int cid, ushort x)
{
    if(bid < m_batteries.size()){
        BMS_BMUDevice *info = m_batteries.at(bid);
        if(cid < 12){
            info->cellVoltage(cid,x);
        }
    }
}

ushort BMS_StackInfo::queueData(int bid, int cid)
{
    if(bid < m_batteries.size()){
        BMS_BMUDevice *info = m_batteries.at(bid);
        if(cid < 12){
            return info->getCellVoltage(cid);
        }
        else if(cid < 17){
            return info->getPackTemperature(cid-12);
        }
        else if(cid < 29){
            return info->balancing().at(cid-17);
        }
    }
    return 0;
}
void BMS_StackInfo::queueData(int bid, int cid, ushort x)
{
    if(bid < m_batteries.size()){
        BMS_BMUDevice *info = m_batteries.at(bid);
        if(cid < 12){
            info->cellVoltage(cid,x);
        }
        else if(cid < 17){
            info->packTemperature(cid-12,x);
        }
    }
}

void BMS_StackInfo::addBattery(BMS_BMUDevice *battery)
{
    this->m_batteries.append(battery);
}

void BMS_StackInfo::setHVC(BMS_HVCInfo *hvc)
{
    m_hvcInfo = hvc;
}


/****** BMS_SystemInfo **********/
BMS_SystemInfo::BMS_SystemInfo(QObject *parent)
{

}

void BMS_SystemInfo::SetStackInfo(QList<BMS_StackInfo*> info)
{
    m_stacks = info;
}

void BMS_SystemInfo::AddStack(BMS_StackInfo *stack)
{
    m_stacks.append(stack);
}

void BMS_SystemInfo::ClearStack()
{
    m_stacks.clear();
}

BMS_StackInfo* BMS_SystemInfo::findStack(QString stackName)
{
    BMS_StackInfo *stack = nullptr;
    foreach(BMS_StackInfo *s, m_stacks){
        if(s->alias() == stackName){
            stack = s;
        }
    }
    return stack;
}

bool BMS_SystemInfo::Configuration(QByteArray data)
{
    bool ret = false;
    QJsonParseError e;
    QJsonDocument d = QJsonDocument::fromJson(data,&e);
    if(d.isNull()){
        return false;
    }
    QJsonObject obj = d.object();

    this->Alias = (obj["alias"].toString());
    this->connectionString = obj["host_ip"].toString();

//    if(obj.contains("canbus")){
//        QJsonArray o = obj["canbus"].toArray();
//        for(auto v:o){
//            QJsonObject q = v.toObject();
//            CANBUSDevice *dev = new CANBUSDevice();
//            dev->name = q["name"].toString();
//            dev->bitrate = q["bitrate"].toInt();
//            QJsonArray a = q["groups"].toArray();
//            for(int i=0;i<a.size();i++){
//                dev->groupList.append(a[i].toInt());
//            }
//            this->m_canbusDevice.append(dev);
//        }
//    }

//    if(obj.contains("mbslave")){
//        QJsonObject o = obj["mbslave"].toObject();
//        this->m_modbusDev = new MODBUSDevice();
//        this->m_modbusDev->bitrate = o["bitrate"].toInt();
//        this->m_modbusDev->portName = o["port"].toString();
//    }

    if(obj.contains("balancing")){
        QJsonObject bal = obj["balancing"].toObject();
        this->BalancingVoltage = bal["voltage"].toInt();
        this->BalancingHystersis = bal["hystersis"].toInt();
        this->BalancingOnTime = bal["on_time"].toInt();
        this->BalancingOffTime = bal["off_time"].toInt();
    }

    if(obj.contains("report")){
        QJsonObject rpt = obj["report"].toObject();
        this->normalReportMS = rpt["normal"].toInt();
        this->errorReportMS = rpt["error"].toInt();
    }

    if(obj.contains("stacks")){
        m_stackConfig.clear();
        QJsonArray ja = obj["stacks"].toArray();
        Stacks = 0;
        foreach (QJsonValue o, ja) {
            BMS_StackConfig *cfg = new BMS_StackConfig();
            cfg->m_nofBatteries = o["baterries_per_stack"].toInt();
            cfg->m_nofCellPerBattery = o["cells_per_battery"].toInt();
            cfg->m_nofNTCPerBattery = o["ntcs_per_stack"].toInt();
            cfg->m_hvboard = o["hv_module"].toBool();
            cfg->m_groupID = o["group_id"].toInt();

            m_stackConfig.append(cfg);
            Stacks++;
        }
        ret = true;
    }
    else if(obj.contains("stack")){
        QJsonObject ja = obj["stack"].toObject();
        this->Stacks = ja["count"].toInt();
        for(int i=0;i<Stacks;i++){
            BMS_StackConfig *cfg = new BMS_StackConfig();
            cfg->m_nofBatteries = ja["baterries_per_stack"].toInt();
            cfg->m_nofCellPerBattery = ja["cells_per_battery"].toInt();
            cfg->m_nofNTCPerBattery = ja["ntcs_per_stack"].toInt();
            cfg->m_hvboard = ja["hv_module"].toBool();
            cfg->m_groupID = i+1;
            m_stackConfig.append(cfg);
        }
        ret = true;
    }
    if(obj.contains("bcu")){
        this->m_bcuDevice = new BMS_BCUDevice();
        QJsonObject bcu = obj["bcu"].toObject();
        if(bcu.contains("digital")){
            QJsonObject digital = bcu["digital"].toObject();
            if(digital.contains("input")){
//                QJsonObject di = digital["input"].toObject();
                int n = digital["input"].toObject()["channels"].toInt();
                m_bcuDevice->add_digital_input(n);
            }
            if(digital.contains("output")){
                int n = digital["output"].toObject()["channels"].toInt();
                m_bcuDevice->add_digital_output(n);
            }

        }
        if(bcu.contains("analog")){
            QJsonObject analog = bcu["anlog"].toObject();
            if(analog.contains("input")){
                int n = analog["input"].toObject()["channels"].toInt();
                m_bcuDevice->add_analog_input(n);
            }
        }
        if(bcu.contains("voltage_source")){
            QJsonObject vs = bcu["voltage_source"].toObject();
            if(vs.contains("channels")){
                int n = vs["channels"].toInt();
                m_bcuDevice->add_voltage_source(n);
            }
        }
    }
    if(ret){
        generateSystemStructure();
    }
    return ret;
}

QByteArray BMS_SystemInfo::Configuration()
{
    QByteArray ret;

    return ret;
}


void BMS_SystemInfo::generateSystemStructure()
{
    m_stacks.clear();
    foreach(BMS_StackConfig *c, m_stackConfig)
    {
        BMS_StackInfo *info = new BMS_StackInfo();
        for(int i=0;i<c->m_nofBatteries;i++){
            BMS_BMUDevice *bat = new BMS_BMUDevice(c->m_nofCellPerBattery,c->m_nofNTCPerBattery);
            info->addBattery(bat);
        }
        info->enableHVModule();
//        if(c->m_hvboard){
//            info->enableHVModule();
//        }
        info->groupID(c->m_groupID);

        m_stacks.append(info);
    }

}

void BMS_SystemInfo::generateDummySystem()
{
    qDebug()<<"generate dummy system";
    m_stacks.clear();
    for(int i=1;i<8;i++) // 7 stacks
    {
        BMS_StackInfo *info = new BMS_StackInfo();
        for(int j=0;j<20;j++){
            BMS_BMUDevice *bat = new BMS_BMUDevice(12,5);
            bat->deviceID(GROUP(i)+j+2);
            info->addBattery(bat);
        }
        info->enableHVModule();
        info->groupID((i));

        m_stacks.append(info);
    }
}

void BMS_SystemInfo::feedData(quint32 identifier, QByteArray data)
{
    foreach(BMS_StackInfo *s,m_stacks){
        s->feedData(identifier,data);
    }
}

void BMS_SystemInfo::startSimulator(int interval)
{
    m_simulateTimer = new QTimer();
    connect(m_simulateTimer,&QTimer::timeout,this,&BMS_SystemInfo::simulate);
    m_simulateTimer->start(interval);
}

void BMS_SystemInfo::stopSimulator()
{
    m_simulateTimer->stop();
    m_simulateTimer->deleteLater();
}

void BMS_SystemInfo::simulate()
{
    //feed data to stacks
    ushort sim_data[4];
    QByteArray data;
    QDataStream ds(&data, QIODevice::ReadWrite);


    foreach(BMS_StackInfo *s,m_stacks){
        s->dummyData();
    }


}

QByteArray BMS_SystemInfo::data()
{
    // loop through each stack to gather data
    QByteArray d;
    QDataStream s(&d,QIODevice::WriteOnly);
    foreach(BMS_StackInfo *b,m_stacks){
        s << b->data();
    }

    return d;
}


