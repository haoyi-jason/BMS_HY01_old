#include "bms_stack.h"
#include "bms_def.h"
#include "bms_bmudevice.h"
#include "bms_bcudevice.h"
#include "bms_svidevice.h"


BMS_StackInfo::BMS_StackInfo(QObject *parent) : QObject(parent),
    m_soc(0),m_soh(0),m_groupID(0),m_State("Idle"),m_StackVoltage(0),
    m_StackCurrent(0),m_MaxCellIndex(-1),m_MinCellIndex(-1),
    m_MaxTemperature(0),m_MinTemperature(0)
{

}

int BMS_StackInfo::BatteryCount(){return m_batteries.size();}
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
void BMS_StackInfo::cellVoltage(int bid, int cid, ushort x){
    if(bid < m_batteries.size()){
        BMS_BMUDevice *info = m_batteries.at(bid);
        if(cid < 12){
            info->cellVoltage(cid,x);
        }
    }
}

ushort BMS_StackInfo::packTemp(int bid, int tid){};
void BMS_StackInfo::packTemp(int bid, int tid, ushort x){};
ushort BMS_StackInfo::queueData(int bid, int cid){
    if(bid < m_batteries.size()){
        BMS_BMUDevice *info = m_batteries.at(bid);
        int m = info->cellCount();
        int n = info->ntcCount();
        if(cid < m){
            return info->getCellVoltage(cid);
        }
        else if(cid < (m+n)){
            return info->getPackTemperature(cid-info->cellCount());
        }
        else if(cid < (m+m+n)){
            return info->totalVoltage();
//            return info->balancing().at(cid-info->cellCount() - info->ntcCount());
        }
    }
    return 0;
}
void BMS_StackInfo::queueData(int bid, int cid, ushort x){
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

void BMS_StackInfo::addBattery(BMS_BMUDevice *battery){ this->m_batteries.append(battery);}
void BMS_StackInfo::setHVC(BMS_HVCInfo *hvc){    m_hvcInfo = hvc;}
ushort BMS_StackInfo::soc(){return m_soc;}
void BMS_StackInfo::set_soc(ushort soc){m_soc = soc;}
ushort BMS_StackInfo::soh(){return m_soh;}
void BMS_StackInfo::soh(ushort x){m_soh = x;}
ushort BMS_StackInfo::maxCellVoltage(){return m_MaxCellVoltage;}
void BMS_StackInfo::maxCellVoltage(ushort x){m_MaxCellVoltage = x;}
int BMS_StackInfo::maxCellVoltIndex(){return m_MaxCellIndex;}
void BMS_StackInfo::maxCellVoltIndex(int x){m_MaxCellIndex = x;}

ushort BMS_StackInfo::minCellVoltage(){return m_MinCellVoltage;}
void BMS_StackInfo::minCellVoltage(ushort x){m_MinCellVoltage = x;}
int BMS_StackInfo::minCellVoltIndex(){return m_MinCellIndex;}
void BMS_StackInfo::minCellVoltIndex(int x){m_MinCellIndex = x;}

ushort BMS_StackInfo::maxStackTemperature(){return m_MaxTemperature;}
void BMS_StackInfo::maxStackTemperature(ushort x){m_MaxTemperature = x;}
ushort BMS_StackInfo::minStackTemperature(){return m_MinTemperature;}
void BMS_StackInfo::minStackTemperature(ushort x){m_MinTemperature = x;}
quint32 BMS_StackInfo::stackVoltage(){return m_StackVoltage;}
void BMS_StackInfo::stackVoltage(quint32 x){m_StackVoltage = x;}
short BMS_StackInfo::stackCurrent(){return m_StackCurrent;}
void BMS_StackInfo::stackCurrent(short x){m_StackCurrent = x;}
QString BMS_StackInfo::alias(){return m_alias;}
void BMS_StackInfo::alias(QString x){m_alias = x;}
int BMS_StackInfo::groupID(){return m_groupID;}
void BMS_StackInfo::groupID(int value){m_groupID = value;}
void BMS_StackInfo::enableHVModule(){m_hvcInfo = new BMS_HVCInfo();}
void BMS_StackInfo::feedData(quint32 identifier, QByteArray data){
    uint8_t id = (identifier >> 12) & 0xff;
    uint16_t cmd = (identifier & 0xFFF);

    if(GROUP_OF(id) == m_groupID){
        if(ID_OF(id) == SVI_ID){ // SVI Module
            this->m_hvcInfo->feedData(ID_OF(id),cmd,data);
        }
        else{
            foreach(BMS_BMUDevice *b, m_batteries){
                b->feedData(ID_OF(id),cmd,data);
            }
        }
    }
}

void BMS_StackInfo::dummyData(){
    quint32 stack_v = 0;
    foreach (BMS_BMUDevice *b, m_batteries) {
        b->dummyData();
        stack_v += b->totalVoltage();
    }
    this->m_StackVoltage = stack_v;
    this->m_StackCurrent = 4000;
    this->m_State = "IDLE";
    this->m_soc = 100;
    this->m_soh = 100;



}
QByteArray BMS_StackInfo::data()
{
    QByteArray d;
    QDataStream s(&d,QIODevice::WriteOnly);

    s << m_groupID;
    s << m_soc;
    s << m_soh;

    foreach(BMS_BMUDevice *b, m_batteries){
        //data.append(b->data());
        //s << b->data();
        s << b;
    }
    if(m_hvcInfo != nullptr){
        //data.append(m_hvcInfo->data());
        s << m_hvcInfo->data();
    }
    return d;
}


QDataStream &operator<<(QDataStream &out, const BMS_StackInfo *stack)
{
    out << stack->m_groupID;
    quint8 nofBat = stack->m_batteries.size();
    out << nofBat;
    //out << stack->m_MaxCellVoltage;
    //out << stack->m_MaxCellIndex;
    out << stack->m_hvcInfo;
    foreach (BMS_BMUDevice *b, stack->m_batteries) {
        out << b;
    }
    out << stack->m_StackVoltage;
    out << stack->m_StackCurrent;
    out << stack->m_soc;
    out << stack->m_soh;
    return out;
}

QDataStream &operator >> (QDataStream &in, BMS_StackInfo *stack)
{
    quint8 nofBat;
    in >> stack->m_groupID;
    in >> nofBat;

    if(stack->m_hvcInfo == nullptr){
        stack->m_hvcInfo = new BMS_HVCInfo();
    }
    in >> stack->m_hvcInfo;

    if(stack->m_batteries.size() == 0){
        for(quint8 i=0;i<nofBat;i++){
            stack->m_batteries.append(new BMS_BMUDevice());
        }
    }



    ushort max_v = 0, max_t = 0;
    ushort min_v = 0xffff, min_t = 0xffff;
    int max_v_index=0, max_t_index = 0;
    int min_v_index=0, min_t_index=0;
    quint32 totalVoltage = 0;
    for(int i=0;i<stack->m_batteries.size();i++){
//        foreach (BMS_BMUDevice *b, stack->m_batteries) {
        BMS_BMUDevice *b = stack->m_batteries[i];
        in >> b;
        if(b->maxCellVoltage()>max_v){
            max_v = b->maxCellVoltage();
            max_v_index = i;
        }
        if(b->minCellVoltage()<min_v){
            min_v = b->minCellVoltage();
            min_v_index = i;
        }
        if(b->maxPackTemp() > max_t){
            max_t = b->maxPackTemp();
            max_t_index = i;
        }
        if(b->minPackTemp() < min_t){
            min_t = b->minPackTemp();
            min_t_index = i;
        }
        totalVoltage += b->totalVoltage();
    }
    in >> stack->m_StackVoltage;
    in >> stack->m_StackCurrent;
    in >> stack->m_soc;
    in >> stack->m_soh;
    if(stack->m_hvcInfo->voltage() != 0){
        stack->m_StackVoltage = stack->m_hvcInfo->voltage();
        stack->m_StackCurrent = stack->m_hvcInfo->current();
    }

    stack->m_MaxCellVoltage = max_v;
    stack->m_MinCellVoltage = min_v;
    stack->m_MaxCellIndex = max_v_index;
    stack->m_MinCellIndex = min_v_index;
    stack->m_MaxTemperature = max_t;
    stack->m_MinTemperature = min_t;
    stack->m_soc = 100;
}

QList<BMS_BMUDevice*> BMS_StackInfo::batteries()
{
    return m_batteries;
}

void BMS_StackInfo::state(QString v)
{
    m_State = v;
}

QString BMS_StackInfo::state()
{
    return m_State;
}
