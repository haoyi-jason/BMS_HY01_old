#include "bms_stack.h"
#include "bms_def.h"
#include "bms_bmudevice.h"
#include "bms_bcudevice.h"
#include "bms_svidevice.h"


BMS_Stack::BMS_Stack(QObject *parent) : QObject(parent),
    m_soc(0),m_soh(0),m_groupID(0),m_State("待機"),m_StackVoltage(0),
    m_StackCurrent(0),m_MaxCellIndex(-1),m_MinCellIndex(-1),
    m_MaxTemperature(0),m_MinTemperature(0)
{

}

BMS_Stack::~BMS_Stack(){
    foreach (BMS_BMUDevice *b, m_batteries) {
        b->deleteLater();
        b = nullptr;
    }
    m_svi->deleteLater();
    m_svi = nullptr;
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
int32_t BMS_Stack::queueData(int bid, int cid){
    if(bid < m_batteries.size()){
        BMS_BMUDevice *info = m_batteries.at(bid);
        int m = info->cellCount();
        int n = info->ntcCount();
        if(cid < m){
            return info->getCellVoltage(cid);
        }
        else if(cid == m){
            return info->totalVoltage();
        }
        else if(cid < (m+n+1)){
            return info->getPackTemperature(cid - m - 1);
        }
    }
    return 0;
}
void BMS_Stack::queueData(int bid, int cid, qint32 x){
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
ushort BMS_Stack::soc()
{
    if(m_svi == nullptr)
        return m_soc;
    else{
        return (ushort)(m_svi->soc()*10);
    }
}
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

quint32 BMS_Stack::stackVoltage()
{
    if(m_svi == nullptr)
        return m_StackVoltage;
    else{
        return m_svi->voltage();
    }
}
void BMS_Stack::stackVoltage(quint32 x){m_StackVoltage = x;}
short BMS_Stack::stackCurrent()
{
    if(m_svi == nullptr)
        return m_StackCurrent;
    else{
        return m_svi->current();
    }
}
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
    if(this->sviDevice() == nullptr){
        this->m_StackVoltage = stack_v/100;
        this->m_StackCurrent = 4000;
        this->m_State = "待機";
        this->m_soc = 100;
        this->m_soh = 100;
    }
    else{
        this->sviDevice()->voltage(stack_v/100);
        this->sviDevice()->simData();
        this->m_StackVoltage = this->sviDevice()->voltage();
        this->m_StackCurrent = this->sviDevice()->current();
        if(this->sviDevice()->current() > 5){
            this->m_State = "充電";
        }
        else if(this->sviDevice()->current() <-5){
            this->m_State = "放電";
        }
        else{
            this->m_State = "待機";
        }
        this->m_soc = this->sviDevice()->soc();
        this->m_soh = this->sviDevice()->soh();
    }



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

//void BMS_Stack::setSimStackVoltage(quint8 gid, int v)
//{

//}

//void BMS_Stack::setSimStackAmpere(quint8 gid, int v)
//{

//}

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

void BMS_Stack::valid()
{
    short max_v = 0, max_t = 0;
    short min_v = 0x7fff, min_t = 0x7fff;
    int max_v_index=0, max_t_index = 0;
    int min_v_index=0, min_t_index=0;
    ushort min_cid = 0xff, max_cid = 0xff;
    short min_tid = 0xff, max_tid = 0xff;
    quint32 totalVoltage = 0;
    for(int i=0;i<m_batteries.size();i++){
        BMS_BMUDevice *b = m_batteries[i];
        b->deviceLost(); // update state
        if(b->maxCellVoltage()>max_v){
            max_v = b->maxCellVoltage();
            max_v_index = i;
            max_cid = b->maxCID();
        }
        if(b->minCellVoltage()<min_v){
            min_v = b->minCellVoltage();
            min_v_index = i;
            min_cid =  b->minCID();
        }
        if(b->maxPackTemp() > max_t){
            max_t = b->maxPackTemp();
            max_t_index = i;
            max_tid = b->maxTID();
        }
        if(b->minPackTemp() < min_t){
            min_t = b->minPackTemp();
            min_t_index = i;
            min_tid = b->minTID();
        }
        totalVoltage += b->totalVoltage();
    }
    m_MaxCellVoltage = max_v;
    m_MinCellVoltage = min_v;
    m_MaxCellIndex = max_cid;
    m_MinCellIndex = min_cid;
    m_MaxTemperature = max_t;
    m_MinTemperature = min_t;
    m_CellVoltDiff = max_v - min_v;



    m_MaxVBID = max_v_index;
    m_MinVBID = min_v_index;
    m_MaxTBID = max_t_index;
    m_MinTBID = min_t_index;

    m_MaxVCID = max_cid;
    m_MinVCID = min_cid;
    m_MaxTCID = max_tid;
    m_MinTCID = min_tid;

    m_MaxCV = max_v;
    m_MinCV = min_v;
    m_MaxPT = max_t;
    m_MinPT = min_t;
}

int BMS_Stack::maxCTBID(){return m_MaxTBID;}
int BMS_Stack::minCTBID(){return m_MinTBID;}
int BMS_Stack::maxCVBID(){return m_MaxVBID;}
int BMS_Stack::minCVBID(){return m_MinVBID;}
int BMS_Stack::maxCTTID(){return m_MaxTCID;}
int BMS_Stack::minCTTID(){return m_MinTCID;}
int BMS_Stack::maxCVCID(){return m_MaxVCID;}
int BMS_Stack::minCVCID(){return m_MinVCID;}

short BMS_Stack::maxStackCV(){return m_MaxCV;}
short BMS_Stack::minStackCV(){return m_MinCV;}
short BMS_Stack::maxStackPT(){return m_MaxPT;}
short BMS_Stack::minStackPT(){return m_MinPT;}

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
    quint32 sv = stack->m_StackVoltage + stack->m_simStackVShift;
    short sa = stack->m_StackCurrent + stack->m_simStackAShift;
    out << sv;
    out << sa;
    out << stack->m_soc;
    out << stack->m_soh;

    out << stack->m_MaxTBID;
    out << stack->m_MinTBID;
    out << stack->m_MaxVBID;
    out << stack->m_MinVBID;
    out << stack->m_MaxTCID;
    out << stack->m_MinTCID;
    out << stack->m_MaxVCID;
    out << stack->m_MinVCID;
    out << stack->m_MaxCV;
    out << stack->m_MinCV;
    out << stack->m_MaxPT;
    out << stack->m_MinPT;

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
    for(int i=0;i<stack->m_batteries.size();i++){
        BMS_BMUDevice *b = stack->m_batteries[i];
        in >> b;
    }
    in >> stack->m_StackVoltage;
    in >> stack->m_StackCurrent;
    in >> stack->m_soc;
    in >> stack->m_soh;
    if(stack->m_svi->voltage() != 0){
        stack->m_StackVoltage = stack->m_svi->voltage();
        stack->m_StackCurrent = stack->m_svi->current();
    }

    in >> stack->m_MaxTBID;
    in >> stack->m_MinTBID;
    in >> stack->m_MaxVBID;
    in >> stack->m_MinVBID;
    in >> stack->m_MaxTCID;
    in >> stack->m_MinTCID;
    in >> stack->m_MaxVCID;
    in >> stack->m_MinVCID;
    in >> stack->m_MaxCV;
    in >> stack->m_MinCV;
    in >> stack->m_MaxPT;
    in >> stack->m_MinPT;
    //qDebug()<<"Feed into Stack struct:"<<min_t;


    //stack->m_soc = 100;
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
    int id = 0;
    ushort max_v = 0, max_t = 0;
    ushort min_v = 0xffff, min_t = 0xffff;
    int max_v_index=0, max_t_index = 0;
    int min_v_index=0, min_t_index=0;
    quint32 totalVoltage = 0;

    foreach(BMS_BMUDevice *b,m_batteries){
        if(b->isOT()) alarm |= (1 << bms::CELL_OT);
        if(b->isUT()) alarm |= (1 << bms::CELL_UT);
        if(b->isOV()) alarm |= (1 << bms::CELL_OV);
        if(b->isUV()) alarm |= (1 << bms::CELL_UV);
        if(b->isOTA()) alarm |= (1 << bms::CELL_OTA);
        if(b->isUTA()) alarm |= (1 << bms::CELL_UTA);
        if(b->isOVA()) alarm |= (1 << bms::CELL_OVA);
        if(b->isUVA()) alarm |= (1 << bms::CELL_UVA);
        if(b->isLost()){
            alarm |= (1 <<bms::BMU_LOST);
            //qDebug()<< QString("BMU %1 Lost").arg(id);
        }
        if(b->maxCellVoltage()>max_v){
            max_v = b->maxCellVoltage();
            max_v_index = id;
        }
        if(b->minCellVoltage()<min_v){
            min_v = b->minCellVoltage();
            min_v_index = id;
        }
        if(b->maxPackTemp() > max_t){
            max_t = b->maxPackTemp();
            max_t_index = id;
        }
        if(b->minPackTemp() < min_t){
            min_t = b->minPackTemp();
            min_t_index = id;
        }
        id++;
    }
    m_MaxCellVoltage = max_v;
    m_MinCellVoltage = min_v;
    m_MaxCellIndex = max_v_index;
    m_MinCellIndex = min_v_index;
    m_MaxTemperature = max_t;
    m_MinTemperature = min_t;
    m_CellVoltDiff = max_v - min_v;

    if(m_svi->isOvWarning())
        alarm |= (1 << bms::STACK_OV);
    if(m_svi->isUvWarning())
        alarm |= (1 << bms::STACK_UV);
    if(m_svi->deviceLost())
        alarm |= (1 << bms::SVI_LOST);

    if(m_svi->isOvAlarm())
        alarm |= (1 << bms::STACK_OVA);
    if(m_svi->isUvAlarm())
        alarm |= (1 << bms::STACK_UVA);

    if(m_svi->isSOCWarning())
        alarm |= (1 << bms::SOC_LOW_W);
    if(m_svi->isSOCAlarm())
        alarm |= (1 << bms::SOC_LOW_A);

    return alarm;
}

void BMS_Stack::clearAlarm()
{
    foreach(BMS_BMUDevice *s, m_batteries){
        s->clearAlarm();
    }
    m_svi->clearAlarm();
}
