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
    int sv = m_stackVoltage + m_simVolt;
    if(m_ovWarningEn){
        if(sv > m_ovWarningSet){
            m_ovWarningSetCntr++;
        }
        else{
            m_ovWarningSetCntr = 0;
        }
        if(sv < m_ovWarningClr){
            m_ovWarningClrCntr++;
        }
        else{
            m_ovWarningClrCntr = 0;
        }
    }
    if(m_ovAlarmEn){
        if(sv > m_ovAlarmSet){
            m_ovAlarmSetCntr++;
        }
        else{
            m_ovAlarmSetCntr = 0;
        }
        if(sv < m_ovAlarmClr){
            m_ovAlarmClrCntr++;
        }
        else{
            m_ovAlarmClrCntr = 0;
        }
    }
    if(m_uvWarningEn){
        if(sv < m_uvWarningSet){
            m_uvWarningSetCntr++;
        }
        else{
            m_uvWarningSetCntr = 0;
        }
        if(sv > m_uvWarningClr){
            m_uvWarningClrCntr++;
        }
        else{
            m_uvWarningClrCntr = 0;
        }
    }

    if(m_uvAlarmEn){
        if(sv < m_uvAlarmSet){
            m_uvAlarmSetCntr++;
        }
        else{
            m_uvAlarmSetCntr = 0;
        }
        if(sv > m_uvAlarmClr){
            m_uvAlarmClrCntr++;
        }
        else{
            m_uvAlarmClrCntr = 0;
        }
    }

}

int BMS_SVIDevice::ovAlarm()
{
    if(!m_ovAlarmEn) return 1;
    bool set =false;
    bool reset = false;
    int ret = 0;
    if(m_ovAlarmSetCntr > m_holdSec){
        set = true;
    }
    else if(m_ovAlarmClrCntr > m_holdSec){
        reset = true;
    }
    if(m_ovAlarmIsSet){
        if(set){
            ret = 0;
        }
        else if(reset){
            ret = 2;
            m_ovAlarmIsSet = false;
        }
        else{
            ret = 0;
        }
    }
    else{
        if(set){
            ret = 1;
            m_ovAlarmIsSet = true;
        }
        else if(reset){
            ret = 0;
        }
        else{
            ret = 0;
        }
    }
    return ret;
}

/**
 * @brief BMS_SVIDevice::ovWarning
 * @return 0: no action, 1:set, 2: reset
 */

int BMS_SVIDevice::ovWarning()
{
    if(!m_ovWarningEn) return 0;
    bool set =false;
    bool reset = false;
    int ret = 0;
    if(m_ovWarningSetCntr > m_holdSec){
        set = true;
    }
    else if(m_ovWarningClrCntr > m_holdSec){
        reset = true;
    }
    if(m_ovWarningIsSet){
        if(set){
            ret = 0;
        }
        else if(reset){
            ret = 2;
            m_ovWarningIsSet = false;
        }
        else{
            ret = 0;
        }
    }
    else{
        if(set){
            ret = 1;
            m_ovWarningIsSet = true;
        }
        else if(reset){
            ret = 0;
        }
        else{
            ret = 0;
        }
    }
    return ret;
}

int BMS_SVIDevice::uvAlarm(){
    if(!m_uvAlarmEn) return 0;
    bool set =false;
    bool reset = false;
    int ret = 0;
    if(m_uvAlarmSetCntr > m_holdSec){
        set = true;
    }
    else if(m_uvAlarmClrCntr > m_holdSec){
        reset = true;
    }
    if(m_ovAlarmIsSet){
        if(set){
            ret = 0;
        }
        else if(reset){
            ret = 2;
            m_ovAlarmIsSet = false;
        }
        else{
            ret = 0;
        }
    }
    else{
        if(set){
            ret = 1;
            m_ovAlarmIsSet = true;
        }
        else if(reset){
            ret = 0;
        }
        else{
            ret = 0;
        }
    }
    return ret;
}

int BMS_SVIDevice::uvWarning()
{
    if(!m_uvWarningEn) return 0;
    bool set =false;
    bool reset = false;
    int ret = 0;

    if(m_uvWarningSetCntr > m_holdSec){
        set = true;
    }
    else if(m_uvWarningClrCntr > m_holdSec){
        reset = true;
    }
    if(m_uvWarningIsSet){
        if(set){
            ret = 0;
        }
        else if(reset){
            ret = 2;
            m_uvWarningIsSet = false;
        }
        else{
            ret = 0;
        }
    }
    else{
        if(set){
            ret = 1;
            m_uvWarningIsSet = true;
        }
        else if(reset){
            ret = 0;
        }
        else{
            ret = 0;
        }
    }
    return ret;
}



void BMS_SVIDevice::clearAlarm()
{
    m_ovAlarmClrCntr = 0;
    m_ovAlarmSetCntr = 0;
    m_ovWarningClrCntr = 0;
    m_ovWarningSetCntr = 0;
    m_uvAlarmClrCntr = 0;
    m_uvAlarmSetCntr = 0;
    m_uvWarningClrCntr = 0;
    m_uvWarningSetCntr = 0;

    m_ovAlarmIsSet = false;
    m_ovWarningIsSet = false;
    m_uvAlarmIsSet = false;
    m_uvWarningIsSet = false;
}

void BMS_SVIDevice::simData()
{
    m_currentTime = QDateTime::currentMSecsSinceEpoch();
    this->validAlarm();
    this->calculateState();
}

QDataStream& operator << (QDataStream &out, const BMS_SVIDevice *svi)
{
    int sv = svi->m_stackVoltage + svi->m_simVolt;
    int sa = svi->m_stackCurrent + svi->m_simAmpere;
    float soc = svi->m_soc + svi->m_simSOC;
    out << svi->m_lastSeen;
    out << sv;
    out << sa;
    out << (svi->m_soh);
    out << soc;
    out << svi->m_sohTrack;
    out << svi->m_ovAlarmIsSet;
    out << svi->m_ovWarningIsSet;
    out << svi->m_uvAlarmIsSet;
    out << svi->m_uvWarningIsSet;
//    out << svi->m_ovAlarmEn;
//    out << svi->m_ovWarningEn;
//    out << svi->m_uvAlarmEn;
//    out << svi->m_uvWarningEn;
//    out << svi->m_ovAlarmSetCntr;
//    out << svi->m_ovAlarmClrCntr;
//    out << svi->m_ovWarningSetCntr;
//    out << svi->m_ovWarningClrCntr;
//    out << svi->m_uvAlarmSetCntr;
//    out << svi->m_uvAlarmClrCntr;
//    out << svi->m_uvWarningSetCntr;
//    out << svi->m_uvWarningClrCntr;
    return out;
}

QDataStream& operator >> (QDataStream &in, BMS_SVIDevice *svi)
{
    in >> svi->m_lastSeen;
    in >> svi->m_stackVoltage;
    in >> svi->m_stackCurrent;
    in >> svi->m_soh;
    in >> svi->m_soc;
    in >> svi->m_sohTrack;
    in >> svi->m_ovAlarmIsSet;
    in >> svi->m_ovWarningIsSet;
    in >> svi->m_uvAlarmIsSet;
    in >> svi->m_uvWarningIsSet;
    return in;
}
