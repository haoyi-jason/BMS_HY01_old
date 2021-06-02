#include "bms_svidevice.h"
#include "bms_def.h"

BMS_HVCInfo::BMS_HVCInfo(QObject *parent) : QObject(parent)
{

}

QByteArray BMS_HVCInfo::data(){
    QByteArray d;
    QDataStream s(&d,QIODevice::WriteOnly);
    s << m_stackVoltage;
    s << m_stackCurrent;
    return d;
}

int BMS_HVCInfo::voltage(){return m_stackVoltage;}
int BMS_HVCInfo::current(){return m_stackCurrent;}

void BMS_HVCInfo::feedData(quint8 id, quint16 msg, QByteArray data){
    if(id == SVI_ID){
        QDataStream ds(&data,QIODevice::ReadOnly);
        ds.setByteOrder(QDataStream::LittleEndian);
        quint16 v;
        ds >> v;
        m_stackVoltage = v;
        ds >> v;
        m_stackCurrent = v;
    }
}
QDataStream& operator << (QDataStream &out, const BMS_HVCInfo *hvc)
{
    out << hvc->m_stackVoltage;
    out << hvc->m_stackCurrent;
    return out;
}

QDataStream& operator >> (QDataStream &in, BMS_HVCInfo *hvc)
{
    in >> hvc->m_stackVoltage;
    in >> hvc->m_stackCurrent;
    return in;
}
