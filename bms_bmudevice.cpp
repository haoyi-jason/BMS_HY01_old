#include "bms_bmudevice.h"
#include "bms_def.h"

BMS_BMUDevice::BMS_BMUDevice(QObject *parent) : QObject(parent)
{
    for(int i=0;i<bms::MAX_CELLS;i++){
        m_cellVoltage.append(0);
        m_balancing.append(0);
        m_openWire.append(0);
    }
    for(int i=0;i<bms::MAX_NTC;i++){
        m_packTemperature.append(0);
    }
    m_lastSeen = QDateTime::currentMSecsSinceEpoch();

}

BMS_BMUDevice::BMS_BMUDevice(int nofCells, int nofTemp,QObject *parent):QObject(parent)
{
    m_nofCell = nofCells;
    m_nofNtc = nofTemp;
    for(int i=0;i<nofCells;i++){
        m_cellVoltage.append(0);
        m_balancing.append(0);
        m_openWire.append(0);
        m_simVoltBase.append(0);
    }
    for(int i=0;i<nofTemp;i++){
        m_packTemperature.append(0);
        m_simTempBase.append(0);
    }
}

short BMS_BMUDevice::getCellVoltage(int index)
{
    if(index < m_cellVoltage.size()){
        return m_cellVoltage.at(index);
    }
    return 0;
}
short BMS_BMUDevice::getPackTemperature(int index)
{
    if(index < m_packTemperature.size()){
        return m_packTemperature.at((index));
    }
    return 0;
}

short BMS_BMUDevice::cellVoltage(int index)
{
    return (index<m_cellVoltage.size())?m_cellVoltage[index]:0;
}
QList<short> BMS_BMUDevice::cellVoltages()
{
    return m_cellVoltage;
}
void BMS_BMUDevice::cellVoltage(int id, short x)
{
    m_cellVoltage[id] = x;
}
void BMS_BMUDevice::cellVoltages(QList<short>x)
{
    m_cellVoltage = x;
}

short BMS_BMUDevice::packTemperature(int index)
{
    return (index<m_packTemperature.size())?m_packTemperature[index]:0;
}
QList<short> BMS_BMUDevice::packTemperatures()
{
    return m_packTemperature;
}
void BMS_BMUDevice::packTemperature(int id, short x)
{
    m_packTemperature[id] = x;
}
void BMS_BMUDevice::packTemperatures(QList<short>x)
{
    m_packTemperature = x;
}

ushort BMS_BMUDevice::balancing(int index)
{
    return (index<m_balancing.size())?m_balancing[index]:0;
}
QList<ushort> BMS_BMUDevice::balancing()
{
    return m_balancing;
}
void BMS_BMUDevice::balancing(QList<ushort>x){
    m_balancing = x;
}
void BMS_BMUDevice::balancing(int index, ushort x)
{
    (index < m_balancing.size())?m_balancing[index]=x:0;
}

ushort BMS_BMUDevice::openWire(int index)
{
    return (index<m_openWire.size())?m_openWire[index]:0;
}


void BMS_BMUDevice::balancingVoltage(ushort value)
{
    m_balancingVoltage = value;
}
void BMS_BMUDevice::balancingHystersis(ushort value)
{
    m_balancingHystersis = value;
}
void BMS_BMUDevice::balancingONTime(ushort value)
{
    m_balancingONSeconds = value;
}
void BMS_BMUDevice::balancingOFFTime(ushort value)
{
    m_balancingOFFSeconds = value;
}
quint8 BMS_BMUDevice::deviceID()
{
    return m_devid;
}

void BMS_BMUDevice::deviceID(quint8 value)
{
    m_devid = value;
}
quint8 BMS_BMUDevice::cellCount(){return m_cellVoltage.size();}
quint8 BMS_BMUDevice::ntcCount(){return m_packTemperature.size();}
short BMS_BMUDevice::minCellVoltage(){return m_CVrules.Min;}
short BMS_BMUDevice::maxCellVoltage(){return m_CVrules.Max;}
qint32 BMS_BMUDevice::totalVoltage(){return m_CVrules.Total;}
short BMS_BMUDevice::minPackTemp(){return (short)m_CTrules.Min;}
short BMS_BMUDevice::maxPackTemp(){return (short)m_CTrules.Max;}
ushort BMS_BMUDevice::maxCID(){return m_CVrules.ID_Max;}
ushort BMS_BMUDevice::minCID(){return m_CVrules.ID_Min;}
ushort BMS_BMUDevice::maxTID(){return m_CTrules.ID_Max;}
ushort BMS_BMUDevice::minTID(){return m_CTrules.ID_Min;}
short BMS_BMUDevice::cellVoltageDiff(){return (m_maxVoltage>m_minVoltage)?(m_maxVoltage-m_minVoltage):0;}
int BMS_BMUDevice::lastSeen(){return QDateTime::currentMSecsSinceEpoch() - m_lastSeen;}

void BMS_BMUDevice::feedData(uint8_t id, uint16_t msg, QByteArray data){
    if(id == m_devid){
        QDataStream ds(&data,QIODevice::ReadOnly);
        ds.setByteOrder(QDataStream::LittleEndian);
        int max_cell = m_cellVoltage.size();
        int max_ntc = m_packTemperature.size();
        if((msg & 0xFF0) == 0x80){ // emg data
            quint8 cid = msg & 0xF; // cell id
            // each packet with 8-byte data for variance of 1-seconds data
            QByteArray vq = data.mid(0,8);
            emit emgData(m_devid,cid,m_cellVoltage[cid],vq);
            return;
        }
        switch (msg) {
        case 0x110:
            for(int i=0;i<4;i++){
                if(i < max_cell){
                    ds >> m_cellVoltage[i];
                    //m_cellVoltage[i] += m_simVoltBase[i];
                    m_cellVoltage[i] = m_simVoltBase[i]==0?m_cellVoltage[i]:m_simVoltBase[i];
                }
            }
            break;
        case 0x111:
            for(int i=4;i<8;i++){
                if(i < max_cell){
                    ds >> m_cellVoltage[i];
//                    m_cellVoltage[i] += m_simVoltBase[i];
                    m_cellVoltage[i] = m_simVoltBase[i]==0?m_cellVoltage[i]:m_simVoltBase[i];
                }
            }
            break;
        case 0x112:
            for(int i=8;i<12;i++){
                if(i < max_cell){
                    ds >> m_cellVoltage[i];
//                    m_cellVoltage[i] += m_simVoltBase[i];
                    m_cellVoltage[i] = m_simVoltBase[i]==0?m_cellVoltage[i]:m_simVoltBase[i];
                }
            }
            break;
        case 0x113:
            for(int i=0;i<4;i++){
                if(i < max_ntc){
                    ds >> m_packTemperature[i];
//                    m_packTemperature[i] += m_simTempBase[i];
                    m_packTemperature[i] = m_simTempBase[i]==0?m_packTemperature[i]:m_simTempBase[i];
                }
            }
            break;
        case 0x114:
            if(4 < max_ntc){
                ds >> m_packTemperature[4];
//                m_packTemperature[4] += m_simTempBase[4];
                m_packTemperature[4] = m_simTempBase[4]==0?m_packTemperature[4]:m_simTempBase[4];
            }
            break;
        case 0x115:{
            ushort balancing,openWire;
            ds >> balancing;
            ds >> openWire;
            m_balancingBit = balancing;
            m_openWireBit = openWire;
            this->validAlarmstate();
        }
            break;
        default:
            break;
        }
    }
}

void BMS_BMUDevice::validAlarmstate()
{
    // find max/min values and over/under threshold
    ushort val_max=0, val_min = 0xffff;
    quint16 ov_mask = 0x0;
    quint16 uv_mask = 0x0;
    quint16 ot_mask = 0x0;
    quint16 ut_mask = 0x0;

    quint16 ov_rst_mask = 0x0;
    quint16 uv_rst_mask = 0x0;
    quint16 ot_rst_mask = 0x0;
    quint16 ut_rst_mask = 0x0;
    qint32 val_sum = 0;

    m_lastSeen = QDateTime::currentMSecsSinceEpoch();
    m_CVrules.valid(m_cellVoltage);
    m_CTrules.valid(m_packTemperature);

    m_maxVoltage = m_CVrules.Max;
    m_minVoltage = m_CVrules.Min;
    m_totalVoltage = m_CVrules.Total;

    m_maxTemperature = m_CTrules.Max;
    m_minTemperature = m_CTrules.Min;

}

quint16 BMS_BMUDevice::ovState(quint16 *set,quint16 *clr)
{
    if(m_ovMask.size() < m_holdSec) return 0x00;
    ushort mask_clr = 0xFFFF;
    ushort mask_set = 0xffff;

    foreach (quint16 v, m_ovMask) {
        mask_set &= v;
    }
    foreach (quint16 v, m_ovClrMask) {
        mask_clr &= v;
    }
    *set = mask_set;
    *clr = mask_clr;

    return mask_set ^ m_ovSetMask;
}

quint16 BMS_BMUDevice::cvWarningOVState(quint16 *set, quint16 *clr)
{
    return m_CVrules.warning_high.State(set,clr);
}
quint16 BMS_BMUDevice::cvWarningUVState(quint16 *set, quint16 *clr)
{
    return m_CVrules.warning_low.State(set,clr);
}
quint16 BMS_BMUDevice::cvAlarmOVState(quint16 *set, quint16 *clr)
{
    return m_CVrules.alarm_high.State(set,clr);
}
quint16 BMS_BMUDevice::cvAlarmUVState(quint16 *set, quint16 *clr)
{
    return m_CVrules.alarm_low.State(set,clr);
}
quint16 BMS_BMUDevice::ctWarningOVState(quint16 *set, quint16 *clr)
{
    return m_CTrules.warning_high.State(set,clr);
}
quint16 BMS_BMUDevice::ctWarningUVState(quint16 *set, quint16 *clr)
{
    return m_CTrules.warning_low.State(set,clr);
}
quint16 BMS_BMUDevice::ctAlarmOVState(quint16 *set, quint16 *clr)
{
    return m_CTrules.alarm_high.State(set,clr);
}
quint16 BMS_BMUDevice::ctAlarmUVState(quint16 *set, quint16 *clr)
{
    return m_CTrules.alarm_low.State(set,clr);
}


quint16 BMS_BMUDevice::uvState(quint16 *set,quint16 *clr)
{
    if(m_uvMask.size() < m_holdSec) return 0x00;

    ushort mask_clr = 0xFFFF;
    ushort mask_set = 0xffff;

    foreach (quint16 v, m_uvMask) {
        mask_set &= v;
    }
    foreach (quint16 v, m_uvClrMask) {
        mask_clr &= v;
    }
    *set = mask_set;
    *clr = mask_clr;

    return mask_set ^ m_uvSetMask;

}

quint16 BMS_BMUDevice::otState(quint16 *set,quint16 *clr)
{
    if(m_otMask.size() < m_holdSec) return 0x0;
    ushort mask_clr = 0xFFFF;
    ushort mask_set = 0xffff;

    foreach (quint16 v, m_otMask) {
        mask_set &= v;
    }
    foreach (quint16 v, m_otClrMask) {
        mask_clr &= v;
    }
    *set = mask_set;
    *clr = mask_clr;

    return mask_set ^ m_otSetMask;
}

quint16 BMS_BMUDevice::utState(quint16 *set,quint16 *clr)
{
    if(m_utMask.size() < m_holdSec) return 0x0;

    ushort mask_clr = 0xFFFF;
    ushort mask_set = 0xffff;

    foreach (quint16 v, m_utMask) {
        mask_set &= v;
    }
    foreach (quint16 v, m_utClrMask) {
        mask_clr &= v;
    }
    *set = mask_set;
    *clr = mask_clr;

    return mask_set ^ m_utSetMask;
}

bool BMS_BMUDevice::isOV()
{
    return (m_CVrules.warning_high.enabledMask != 0x0);
}

bool BMS_BMUDevice::isUV()
{
    return (m_CVrules.warning_low.enabledMask != 0x0);
}
bool BMS_BMUDevice::isOT()
{
    return (m_CTrules.warning_high.enabledMask != 0x0);
}
bool BMS_BMUDevice::isUT()
{
    return (m_CTrules.warning_low.enabledMask != 0x0);
}

bool BMS_BMUDevice::isOVA()
{
    return (m_CVrules.alarm_high.enabledMask != 0x0);
}

bool BMS_BMUDevice::isUVA()
{
    return (m_CVrules.alarm_low.enabledMask != 0x0);
}
bool BMS_BMUDevice::isOTA()
{
    return (m_CTrules.alarm_high.enabledMask != 0x0);
}
bool BMS_BMUDevice::isUTA()
{
    return (m_CTrules.alarm_low.enabledMask != 0x0);
}

void BMS_BMUDevice::simData(ushort vbase, ushort vgap, ushort tbase, ushort tgap){
//    ushort max_val = 0, min_val = 0xffff;
    qint32 total = 0;
    for(int i=0;i<m_cellVoltage.size();i++){
        m_cellVoltage[i] = m_simVoltBase[i]==0?(ushort)(vbase + QRandomGenerator::global()->bounded(vgap)):m_simVoltBase[i];
//        max_val = (max_val > m_cellVoltage[i])?max_val:m_cellVoltage[i];
//        min_val = (min_val < m_cellVoltage[i])?min_val:m_cellVoltage[i];
//        total += m_cellVoltage[i];
    }
//    m_maxVoltage = max_val;
//    m_minVoltage = min_val;
   // m_totalVoltage = total;

//    max_val = 0;min_val = 0xffff;
    for(int i=0;i<m_packTemperature.size();i++){
        m_packTemperature[i] = m_simTempBase[i]==0?(ushort)(tbase + QRandomGenerator::global()->bounded(tgap)): m_simTempBase[i];
//        max_val = (max_val > m_packTemperature[i])?max_val:m_packTemperature[i];
//        min_val = (min_val < m_packTemperature[i])?min_val:m_packTemperature[i];
    }

//    validAlarmstate();
//    m_maxTemperature = max_val;
//    m_minTemperature = min_val;

//    m_lastSeen = QDateTime::currentMSecsSinceEpoch();
    this->validAlarmstate();
}

void BMS_BMUDevice::setSimVolt(quint8 id, quint8 cell, ushort shift)
{
    if(ID_OF(id) == m_devid){
        if(cell < m_simVoltBase.count()){
            m_simVoltBase[cell] = shift;
        }
        else if(cell==0xff){
            for(int i=0;i<m_simVoltBase.size();i++){
                m_simVoltBase[i] = shift;
            }
        }
    }
    else if(id == 0xff){
        for(int i=0;i<m_simVoltBase.size();i++){
            m_simVoltBase[i] = 0x0;
        }
    }
}

void BMS_BMUDevice::setSimTemp(quint8 id, quint8 tid, ushort shift)
{
    if(ID_OF(id) == m_devid){
        if(tid < m_simTempBase.count()){
            m_simTempBase[tid] = shift;
        }
        else if(tid==-1){
            for(int i=0;i<m_simTempBase.size();i++){
                m_simTempBase[i] = shift;
            }
        }
    }
    else if(id == 0xff){
        for(int i=0;i<m_simTempBase.size();i++){
            m_simTempBase[i] = 0;
        }
    }
}


QByteArray BMS_BMUDevice::data(){
    QByteArray d;
    QDataStream s(&d,QIODevice::WriteOnly);
    foreach(ushort u, m_cellVoltage){
        s << u;
    }
    foreach(ushort u,m_packTemperature){
        s << u;
    }
    foreach(ushort u, m_balancing){
        s << u;
    }
    return d;
}
QDataStream &operator<<(QDataStream &out, const BMS_BMUDevice *bat)
{
    out << bat->m_devLost;
    out << bat->m_devid;
    out << bat->m_nofCell;
    out << bat->m_nofNtc;
    out << bat->m_maxVoltage;
    out << bat->m_minVoltage;
    out << bat->m_maxTemperature;
    out << bat->m_minTemperature;
    out << bat->m_totalVoltage;
    out << bat->m_balancingBit;
    out << bat->m_openWireBit;
    out << bat->m_ovSetMask;
    out << bat->m_uvSetMask;
    out << bat->m_otSetMask;
    out << bat->m_utSetMask;

    out << bat->m_CVrules.warning_high.enabledMask;
    out << bat->m_CVrules.warning_low.enabledMask;
    out << bat->m_CVrules.alarm_high.enabledMask;
    out << bat->m_CVrules.alarm_low.enabledMask;
    out << bat->m_CTrules.warning_high.enabledMask;
    out << bat->m_CTrules.warning_low.enabledMask;
    out << bat->m_CTrules.alarm_high.enabledMask;
    out << bat->m_CTrules.alarm_low.enabledMask;
    out << bat->m_CVrules.Max;
    out << bat->m_CVrules.Min;
    out << bat->m_CVrules.ID_Max;
    out << bat->m_CVrules.ID_Min;
    out << bat->m_CTrules.Max;
    out << bat->m_CTrules.Min;
    out << bat->m_CTrules.ID_Max;
    out << bat->m_CTrules.ID_Min;
    out << bat->m_CVrules.Total;


    for(int i=0;i<bat->m_cellVoltage.size();i++){
        out << bat->m_cellVoltage[i];
    }
    for(int i=0;i<bat->m_packTemperature.size();i++){
        out << bat->m_packTemperature[i];
    }
    return out;
}
QDataStream &operator >> (QDataStream &in, BMS_BMUDevice *bat)
{
    in >> bat->m_devLost;
    in >> bat->m_devid;
    in >> bat->m_nofCell;
    in >> bat->m_nofNtc;
    in >> bat->m_maxVoltage;
    in >> bat->m_minVoltage;
    in >> bat->m_maxTemperature;
    in >> bat->m_minTemperature;
    in >> bat->m_totalVoltage;
    in >> bat->m_balancingBit;
    in >> bat->m_openWireBit;
    in >> bat->m_ovSetMask;
    in >> bat->m_uvSetMask;
    in >> bat->m_otSetMask;
    in >> bat->m_utSetMask;

    in >> bat->m_CVrules.warning_high.enabledMask;
    in >> bat->m_CVrules.warning_low.enabledMask;
    in >> bat->m_CVrules.alarm_high.enabledMask;
    in >> bat->m_CVrules.alarm_low.enabledMask;
    in >> bat->m_CTrules.warning_high.enabledMask;
    in >> bat->m_CTrules.warning_low.enabledMask;
    in >> bat->m_CTrules.alarm_high.enabledMask;
    in >> bat->m_CTrules.alarm_low.enabledMask;

    in >> bat->m_CVrules.Max;
    in >> bat->m_CVrules.Min;
    in >> bat->m_CVrules.ID_Max;
    in >> bat->m_CVrules.ID_Min;
    in >> bat->m_CTrules.Max;
    in >> bat->m_CTrules.Min;
    in >> bat->m_CTrules.ID_Max;
    in >> bat->m_CTrules.ID_Min;
    in >> bat->m_CVrules.Total;

   // ushort max_v = 0,max_t = 0;
    //ushort min_v = 0xffff,min_t = 100;
    //bat->m_totalVoltage = 0;
    //ushort tmpHighMask=0,tmpLowMask = 0;
    if(bat->m_cellVoltage.size() == 0){
        bat->m_balancing.clear();
        for(int i=0;i<bat->m_nofCell;i++){
            bat->m_cellVoltage.append(0x0);
            bat->m_balancing.append(0x0);
            bat->m_openWire.append(0x0);
        }
    }
    if(bat->m_packTemperature.size() == 0){
        for(int i=0;i<bat->m_nofNtc;i++){
            bat->m_packTemperature.append(0);
        }
    }

    for(int i=0;i<bat->m_cellVoltage.size();i++){
        in >> bat->m_cellVoltage[i];
    }

   // tmpHighMask = tmpLowMask = 0;
    for(int i=0;i<bat->m_packTemperature.size();i++){
        in >> bat->m_packTemperature[i];
    }
    return in;
}

QByteArray BMS_BMUDevice::ActionPending(){
    QByteArray ret;
    if(m_pendingAction.size() > 0){
        ret = m_pendingAction.first();
        m_pendingAction.removeFirst();
    }
    return ret;
}

CAN_Packet *BMS_BMUDevice::setBalancing(quint16 bv, quint8 bh, quint8 be, quint16 on, quint16 off)
{
    CAN_Packet *ret = nullptr;
    ret = new CAN_Packet();
    ret->Command = 0x132;
    QDataStream ds(&ret->data,QIODevice::WriteOnly);
    ds.setByteOrder(QDataStream::LittleEndian);
    quint16 u16;
    ds << bv;
    ds << bh;
    ds << be;
    u16 = on;
    if(u16 > 40){
        u16 = 40;
    }
    ds << u16;

    if(off < u16){
        ds << u16;
    }
    else{
        ds << off;
    }

    return ret;
}

void BMS_BMUDevice::clearAlarm()
{
//    m_ovMask.clear();
//    m_uvMask.clear();
//    m_otMask.clear();
//    m_utMask.clear();
//    m_ovSetMask = 0x0;
//    m_uvSetMask = 0x0;
//    m_otSetMask = 0x0;
//    m_utSetMask = 0x0;

    m_CVrules.reset();
    m_CTrules.reset();
}

bool BMS_BMUDevice::alarm()
{
    return m_alarm;
}

void BMS_BMUDevice::alarm(bool v)
{
    m_alarm = v;
}

void BMS_BMUDevice::resetValues()
{
    for(int i=0;i<m_cellVoltage.size();i++){
        m_cellVoltage[i] = 0;
    }

    for(int i=0; i< m_packTemperature.size();i++){
        m_packTemperature[i] = 0;
    }
}
