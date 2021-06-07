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
}

BMS_BMUDevice::BMS_BMUDevice(int nofCells, int nofTemp,QObject *parent):QObject(parent)
{
    m_nofCell = nofCells;
    m_nofNtc = nofTemp;
    for(int i=0;i<nofCells;i++){
        m_cellVoltage.append(0);
        m_balancing.append(0);
        m_openWire.append(0);
    }
    for(int i=0;i<nofTemp;i++){
        m_packTemperature.append(0);
    }
}

ushort BMS_BMUDevice::getCellVoltage(int index)
{
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

ushort BMS_BMUDevice::cellVoltage(int index)
{
    return (index<m_cellVoltage.size())?m_cellVoltage[index]:0;
}
QList<ushort> BMS_BMUDevice::cellVoltages()
{
    return m_cellVoltage;
}
void BMS_BMUDevice::cellVoltage(int id, ushort x)
{
    m_cellVoltage[id] = x;
}
void BMS_BMUDevice::cellVoltages(QList<ushort>x)
{
    m_cellVoltage = x;
}

ushort BMS_BMUDevice::packTemperature(int index)
{
    return (index<m_packTemperature.size())?m_packTemperature[index]:0;
}
QList<ushort> BMS_BMUDevice::packTemperatures()
{
    return m_packTemperature;
}
void BMS_BMUDevice::packTemperature(int id, ushort x)
{
    m_packTemperature[id] = x;
}
void BMS_BMUDevice::packTemperatures(QList<ushort>x)
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
ushort BMS_BMUDevice::minCellVoltage(){return m_minVoltage;}
ushort BMS_BMUDevice::maxCellVoltage(){return m_maxVoltage;}
ushort BMS_BMUDevice::totalVoltage(){return m_totalVoltage;}
ushort BMS_BMUDevice::minPackTemp(){return m_minTemperature;}
ushort BMS_BMUDevice::maxPackTemp(){return m_maxTemperature;}
ushort BMS_BMUDevice::cellVoltageDiff(){return (m_maxVoltage>m_minVoltage)?(m_maxVoltage-m_minVoltage):0;}
int BMS_BMUDevice::lastSeen(){return QDateTime::currentMSecsSinceEpoch() - m_lastSeen;}

void BMS_BMUDevice::feedData(uint8_t id, uint16_t msg, QByteArray data){
    if(id == m_devid){
        m_lastSeen = QDateTime::currentMSecsSinceEpoch();
        QDataStream ds(&data,QIODevice::ReadOnly);
        ds.setByteOrder(QDataStream::LittleEndian);
        switch (msg) {
        case 0x110:
            for(int i=0;i<4;i++){
                ds >> m_cellVoltage[i];
            }
            break;
        case 0x111:
            for(int i=0;i<4;i++){
                ds >> m_cellVoltage[i+4];
            }
            break;
        case 0x112:
            for(int i=0;i<4;i++){
                ds >> m_cellVoltage[i+8];
            }
            break;
        case 0x113:
            for(int i=0;i<4;i++){
                ds >> m_packTemperature[i];
            }
            break;
        case 0x114:
            ds >> m_packTemperature[4];
            break;
        case 0x115:{
            ushort balancing,openWire;
            ds >> balancing;
            ds >> openWire;
            for(int i=0;i<m_cellVoltage.count();i++){
                if((balancing & (1 <<i)) == (1 << i)){
                    m_balancing[i] = 1;
                }
                else{
                    m_balancing[i] = 0;
                }
                if((openWire & (1 <<i)) == (1 << i)){
                    m_openWire[i] = 1;
                }
                else{
                    m_openWire[i] = 0;
                }
            }
            // find max/min values
            ushort val_max=0, val_min = 0xffff;
            int val_sum = 0;
            foreach (ushort v, m_cellVoltage) {
                val_max = (val_max > v)?val_max:v;
                val_min = (val_min < v)?val_min:v;
                val_sum += v;
            }
            m_maxVoltage = val_max;
            m_minVoltage = val_min;
            m_totalVoltage = val_sum;
            val_max = 0; val_min = 0xffff;
            foreach (ushort v, m_packTemperature) {
                val_max = (val_max > v)?val_max:v;
                val_min = (val_min < v)?val_min:v;
            }
            m_maxTemperature = val_max;
            m_minTemperature = val_min;
        }
            break;
        default:
            break;
        }
    }
}

void BMS_BMUDevice::dummyData(ushort vbase, ushort vgap, ushort tbase, ushort tgap){
    ushort max_val = 0, min_val = 0xffff;
    ushort total = 0;
    for(int i=0;i<m_cellVoltage.size();i++){
        m_cellVoltage[i] = (ushort)(vbase + QRandomGenerator::global()->bounded(vgap));
        max_val = (max_val > m_cellVoltage[i])?max_val:m_cellVoltage[i];
        min_val = (min_val < m_cellVoltage[i])?min_val:m_cellVoltage[i];
        total += m_cellVoltage[i];
    }
    m_maxVoltage = max_val;
    m_minVoltage = min_val;
    m_totalVoltage = total;

    max_val = 0;min_val = 0xffff;
    for(int i=0;i<m_packTemperature.size();i++){
        m_packTemperature[i] = (ushort)(tbase + QRandomGenerator::global()->bounded(tgap));
        max_val = (max_val > m_packTemperature[i])?max_val:m_packTemperature[i];
        min_val = (min_val < m_packTemperature[i])?min_val:m_packTemperature[i];
    }
    m_maxTemperature = max_val;
    m_minTemperature = min_val;
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
    out << bat->m_lastSeen;
    out << bat->m_devid;
    out << bat->m_nofCell;
    out << bat->m_nofNtc;
    out << bat->m_maxVoltage;
    out << bat->m_minVoltage;
    out << bat->m_maxTemperature;
    out << bat->m_minTemperature;
    out << bat->m_totalVoltage;

    for(int i=0;i<bat->m_cellVoltage.size();i++){
        out << bat->m_cellVoltage[i];
    }
    for(int i=0;i<bat->m_packTemperature.size();i++){
        out << bat->m_packTemperature[i];
    }
    for(int i=0;i<bat->m_balancing.size();i++){
        out << bat->m_balancing[i];
    }
    for(int i=0;i<bat->m_openWire.size();i++){
        out << bat->m_openWire[i];
    }
    return out;
}
QDataStream &operator >> (QDataStream &in, BMS_BMUDevice *bat)
{
    in >> bat->m_lastSeen;
    in >> bat->m_devid;
    in >> bat->m_nofCell;
    in >> bat->m_nofNtc;
    in >> bat->m_maxVoltage;
    in >> bat->m_minVoltage;
    in >> bat->m_maxTemperature;
    in >> bat->m_minTemperature;
    in >> bat->m_totalVoltage;

    ushort max_v = 0,max_t = 0;
    ushort min_v = 0xffff,min_t = 100;
    //bat->m_totalVoltage = 0;
    ushort tmpHighMask=0,tmpLowMask = 0;
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
//        max_v = bat->m_cellVoltage[i]>max_v?bat->m_cellVoltage[i]:max_v;
//        min_v = bat->m_cellVoltage[i]<min_v?bat->m_cellVoltage[i]:min_v;
//        bat->m_totalVoltage += bat->m_cellVoltage[i];
//        if(bat->m_cellVoltage[i] > bat->m_voltHighThreshold){
//            tmpHighMask |= (1 << i);
//        }
//        if(bat->m_cellVoltage[i] < bat->m_voltLowThreshold){
//            tmpLowMask |= (1 << i);
//        }
//        bat->m_voltHighMask = tmpHighMask;
//        bat->m_voltLowMask = tmpLowMask;
    }

    tmpHighMask = tmpLowMask = 0;
    for(int i=0;i<bat->m_packTemperature.size();i++){
        in >> bat->m_packTemperature[i];
//        max_t = bat->m_packTemperature[i]>max_t?bat->m_packTemperature[i]:max_t;
//        min_t = bat->m_packTemperature[i]<min_t?bat->m_packTemperature[i]:min_t;
//        if(bat->m_packTemperature[i] > bat->m_tempHighThreshold){
//            tmpHighMask |= (1 << i);
//        }
//        if(bat->m_packTemperature[i] < bat->m_tempLowThreshold){
//            tmpLowMask |= (1 << i);
//        }
//        bat->m_tempHighMask = (quint8)tmpHighMask;
//        bat->m_tempLowMask = (quint8)tmpLowMask;
    }
    for(int i=0;i<bat->m_balancing.size();i++){
        in >> bat->m_balancing[i];
    }
    for(int i=0;i<bat->m_openWire.size();i++){
        in >> bat->m_openWire[i];
    }
//    bat->m_maxVoltage = max_v;
//    bat->m_maxTemperature = max_t;
//    bat->m_minVoltage = min_v;
//    bat->m_minTemperature = min_t;
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
