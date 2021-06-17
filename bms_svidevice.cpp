#include "bms_svidevice.h"
#include "bms_def.h"

BMS_SVIDevice::BMS_SVIDevice(QObject *parent) : QObject(parent)
{

}

QByteArray BMS_SVIDevice::data(){
    QByteArray d;
    QDataStream s(&d,QIODevice::WriteOnly);
    s << m_stackVoltage;
    s << m_stackCurrent;
    return d;
}

int BMS_SVIDevice::voltage(){return m_stackVoltage;}
int BMS_SVIDevice::current(){return m_stackCurrent;}

void BMS_SVIDevice::feedData(quint8 id, quint16 msg, QByteArray data){
    if(id == SVI_ID){
        long long now = QDateTime::currentMSecsSinceEpoch();
        QDataStream ds(&data,QIODevice::ReadOnly);
        ds.setByteOrder(QDataStream::LittleEndian);
        quint16 v;
        ds >> v;
        m_aux1 = v;
        ds >> v;
        m_aux2 = v;

        ds >> v;
        m_stackCurrent = v;
        ds >> v;
        m_stackVoltage = v;

        // calculate SOC


        this->validAlarm();
        m_lastSeen = now;

    }
}

void BMS_SVIDevice::validAlarm()
{
    if(m_stackVoltage > m_voltHighThreshold){
        m_ovCounts++;
    }
    else{
        m_ovCounts = 0;
    }

    if(m_stackCurrent > m_currentHighThreshold){
        m_ocCounts ++;
    }
    else{
        m_ocCounts = 0;
    }

}

int BMS_SVIDevice::voltageAlarm()
{
    if(m_ovCounts > m_holdSec){
        return 1;
    }
    return 0;
}

int BMS_SVIDevice::currentAlarm()
{
    if(m_ocCounts > m_holdSec){
        return 1;
    }

    return 0;
}

void BMS_SVIDevice::clearAlarm()
{
    m_ovCounts = 0;
    m_ocCounts = 0;
}

QDataStream& operator << (QDataStream &out, const BMS_SVIDevice *hvc)
{
    out << hvc->m_stackVoltage;
    out << hvc->m_stackCurrent;
    return out;
}

QDataStream& operator >> (QDataStream &in, BMS_SVIDevice *hvc)
{
    in >> hvc->m_stackVoltage;
    in >> hvc->m_stackCurrent;
    return in;
}
