#include "bms_svidevice.h"
#include "bms_def.h"

BMS_SVIDevice::BMS_SVIDevice(QObject *parent) : QObject(parent)
{
    m_lastSeen = QDateTime::currentMSecsSinceEpoch();
}

QByteArray BMS_SVIDevice::data(){
    QByteArray d;
    QDataStream s(&d,QIODevice::WriteOnly);
    s << m_stackVoltage;
    s << m_stackCurrent;
    return d;
}

int BMS_SVIDevice::voltage(){return m_stackVoltage + m_simVolt;}
int BMS_SVIDevice::current(){return m_stackCurrent + m_simAmpere;}

void BMS_SVIDevice::feedData(quint8 id, quint16 msg, QByteArray data){
    if(id == SVI_ID){
        m_currentTime = QDateTime::currentMSecsSinceEpoch();
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

        this->validAlarm();
        this->calculateState();
    }
}

void BMS_SVIDevice::calculateState()
{
    // calculate SOC
    float sv = (float)m_stackVoltage/10.;
    float sa = (float)(m_stackCurrent+m_simAmpere)/10.;
    float dt = m_currentTime - m_lastSeen;
    m_lastSeen = m_currentTime;

    float soc_new;

    soc_new = sa * dt / 3600./(m_soh*m_capacity/100) + m_soc;
    if(soc_new >100) soc_new = 100;
    if(soc_new < 0) soc_new = 0;
    m_soc = soc_new;

    if(m_sohTrack){
        m_sohAccum += sa/3600.;
    }

}

void BMS_SVIDevice::setSOHTracking(bool state)
{
    m_sohTrack = state;
    if(m_sohTrack){
        m_sohAccum = 0;
    }
    else{
        m_soh = m_sohAccum/m_capacity*100;
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

void BMS_SVIDevice::simData()
{
    m_currentTime = QDateTime::currentMSecsSinceEpoch();
    this->validAlarm();
    this->calculateState();
}

QDataStream& operator << (QDataStream &out, const BMS_SVIDevice *hvc)
{
    int sv = hvc->m_stackVoltage + hvc->m_simVolt;
    int sa = hvc->m_stackCurrent + hvc->m_simAmpere;
    float soc = hvc->m_soc + hvc->m_simSOC;
    out << hvc->m_lastSeen;
    out << sv;
    out << sa;
    out << (hvc->m_soh);
    out << soc;
    out << hvc->m_sohTrack;
    return out;
}

QDataStream& operator >> (QDataStream &in, BMS_SVIDevice *hvc)
{
    in >> hvc->m_lastSeen;
    in >> hvc->m_stackVoltage;
    in >> hvc->m_stackCurrent;
    in >> hvc->m_soh;
    in >> hvc->m_soc;
    in >> hvc->m_sohTrack;
    return in;
}
