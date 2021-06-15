#include "bms_stack.h"
#include "bms_def.h"
#include "bms_bmudevice.h"
#include "bms_bcudevice.h"
#include "bms_svidevice.h"


BMS_Stack::BMS_Stack(QObject *parent) : QObject(parent),
    m_soc(0),m_soh(0),m_groupID(0),m_State("Idle"),m_StackVoltage(0),
    m_StackCurrent(0),m_MaxCellIndex(-1),m_MinCellIndex(-1),
    m_MaxTemperature(0),m_MinTemperature(0)
{

}

int BMS_Stack::BatteryCount(){return m_batteries.size();}
ushort BMS_Stack::cellVoltage(int bid, int cid)
{
    if(bid < m_batteries.size()){
        BMS_BMUDevice *info = m_batteries.at(bid);
        if(cid < 12){
            return info->getCellVoltage(cid);
        }
    }
    return 0;
}
void BMS_Stack::cellVoltage(int bid, int cid, ushort x){
    if(bid < m_batteries.size()){
        BMS_BMUDevice *info = m_batteries.at(bid);
        if(cid < 12){
            info->cellVoltage(cid,x);
        }
    }
}

ushort BMS_Stack::packTemp(int bid, int tid){};
void BMS_Stack::packTemp(int bid, int tid, ushort x){};
ushort BMS_Stack::queueData(int bid, int cid){
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
void BMS_Stack::queueData(int bid, int cid, ushort x){
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

void BMS_Stack::addBattery(BMS_BMUDevice *battery){ this->m_batteries.append(battery);}
void BMS_Stack::setHVC(BMS_SVIDevice *hvc){    m_svi = hvc;}
ushort BMS_Stack::soc(){return m_soc;}
void BMS_Stack::set_soc(ushort soc){m_soc = soc;}
ushort BMS_Stack::soh(){return m_soh;}
void BMS_Stack::soh(ushort x){m_soh = x;}
ushort BMS_Stack::maxCellVoltage(){return m_MaxCellVoltage;}
void BMS_Stack::maxCellVoltage(ushort x){m_MaxCellVoltage = x;}
int BMS_Stack::maxCellVoltIndex(){return m_MaxCellIndex;}
void BMS_Stack::maxCellVoltIndex(int x){m_MaxCellIndex = x;}

ushort BMS_Stack::minCellVoltage(){return m_MinCellVoltage;}
void BMS_Stack::minCellVoltage(ushort x){m_MinCellVoltage = x;}
int BMS_Stack::minCellVoltIndex(){return m_MinCellIndex;}
void BMS_Stack::minCellVoltIndex(int x){m_MinCellIndex = x;}

ushort BMS_Stack::cellVoltDiff(){return m_CellVoltDiff;}
void BMS_Stack::cellVoltDiff(ushort x){m_CellVoltDiff = x;}


ushort BMS_Stack::maxStackTemperature(){return m_MaxTemperature;}
void BMS_Stack::maxStackTemperature(ushort x){m_MaxTemperature = x;}
ushort BMS_Stack::minStackTemperature(){return m_MinTemperature;}
void BMS_Stack::minStackTemperature(ushort x){m_MinTemperature = x;}
quint32 BMS_Stack::stackVoltage(){return m_StackVoltage;}
void BMS_Stack::stackVoltage(quint32 x){m_StackVoltage = x;}
short BMS_Stack::stackCurrent(){return m_StackCurrent;}
void BMS_Stack::stackCurrent(short x){m_StackCurrent = x;}
QString BMS_Stack::alias(){return m_alias;}
void BMS_Stack::alias(QString x){m_alias = x;}
int BMS_Stack::groupID(){return m_groupID;}
void BMS_Stack::groupID(int value){m_groupID = value;}
void BMS_Stack::enableHVModule(){m_svi = new BMS_SVIDevice();}
void BMS_Stack::feedData(quint32 identifier, QByteArray data){
    uint8_t id = (identifier >> 12) & 0xff;
    uint16_t cmd = (identifier & 0xFFF);

    if(GROUP_OF(id) == m_groupID){
        if(ID_OF(id) == SVI_ID){ // SVI Module
            this->m_svi->feedData(ID_OF(id),cmd,data);
        }
        else{
            foreach(BMS_BMUDevice *b, m_batteries){
                b->feedData(ID_OF(id),cmd,data);
            }
        }
    }
}

void BMS_Stack::simData(){
    quint32 stack_v = 0;
    foreach (BMS_BMUDevice *b, m_batteries) {
        b->simData();
        stack_v += b->totalVoltage();
    }
    this->m_StackVoltage = stack_v;
    this->m_StackCurrent = 4000;
    this->m_State = "IDLE";
    this->m_soc = 100;
    this->m_soh = 100;



}
void BMS_Stack::setSimCellData(quint8 id, quint8 cell, ushort v )
{
    foreach (BMS_BMUDevice *b, m_batteries) {
        b->setSimVolt(id,cell,v);
    }
}

void BMS_Stack::setSimTempData(quint8 id, quint8 cell, ushort v)
{
    foreach (BMS_BMUDevice *b, m_batteries) {
        b->setSimTemp(id,cell,v);
    }
}
QByteArray BMS_Stack::data()
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
    if(m_svi != nullptr){
        //data.append(m_svi->data());
        s << m_svi->data();
    }
    return d;
}


QDataStream &operator<<(QDataStream &out, const BMS_Stack *stack)
{
    out << stack->m_groupID;
    quint8 nofBat = stack->m_batteries.size();
    out << nofBat;
    //out << stack->m_MaxCellVoltage;
    //out << stack->m_MaxCellIndex;
    out << stack->m_svi;
    foreach (BMS_BMUDevice *b, stack->m_batteries) {
        out << b;
    }
    out << stack->m_StackVoltage;
    out << stack->m_StackCurrent;
    out << stack->m_soc;
    out << stack->m_soh;
    return out;
}

QDataStream &operator >> (QDataStream &in, BMS_Stack *stack)
{
    quint8 nofBat;
    in >> stack->m_groupID;
    in >> nofBat;

    if(stack->m_svi == nullptr){
        stack->m_svi = new BMS_SVIDevice();
    }
    in >> stack->m_svi;

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
    if(stack->m_svi->voltage() != 0){
        stack->m_StackVoltage = stack->m_svi->voltage();
        stack->m_StackCurrent = stack->m_svi->current();
    }

    //qDebug()<<"Feed into Stack struct:"<<min_t;

    stack->m_MaxCellVoltage = max_v;
    stack->m_MinCellVoltage = min_v;
    stack->m_MaxCellIndex = max_v_index;
    stack->m_MinCellIndex = min_v_index;
    stack->m_MaxTemperature = max_t;
    stack->m_MinTemperature = min_t;
    stack->m_CellVoltDiff = max_v - min_v;
    stack->m_soc = 100;
}

QList<BMS_BMUDevice*> BMS_Stack::batteries()
{
    return m_batteries;
}

void BMS_Stack::state(QString v)
{
    m_State = v;
}

QString BMS_Stack::state()
{
    return m_State;
}

BMS_SVIDevice *BMS_Stack::sviDevice() const
{
    return m_svi;
}

quint32 BMS_Stack::alarmState()
{
    quint32 alarm = 0x0;
    foreach(BMS_BMUDevice *b,m_batteries){
        if(b->isOT()) alarm |= (1 << bms::CELL_OT);
        if(b->isUT()) alarm |= (1 << bms::CELL_UT);
        if(b->isOV()) alarm |= (1 << bms::CELL_OV);
        if(b->isUV()) alarm |= (1 << bms::CELL_UV);
    }
    return alarm;
}

void BMS_Stack::clearAlarm()
{
    foreach(BMS_BMUDevice *s, m_batteries){
        s->clearAlarm();
    }
    m_svi->clearAlarm();
}
