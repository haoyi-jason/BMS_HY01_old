#include "bms_svidevice.h"
#include "bms_def.h"
#include <QDateTime>

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

//int BMS_SVIDevice::voltage(){return m_stackVoltage + m_simVolt;}
//int BMS_SVIDevice::current(){return m_stackCurrent + m_simAmpere;}

int BMS_SVIDevice::voltage(){
    if(m_simVolt == 0)
        return m_stackVoltage;
    else {
        return m_simVolt;
    }
}
int BMS_SVIDevice::current()
{
    if(m_simAmpere == 0)
        return m_stackCurrent;
    else {
        return m_simAmpere;
    }
}

bool BMS_SVIDevice::deviceLost()
{
    //qDebug()<<"SVI:NOW:"<<QDateTime::currentDateTime().toString("hhMMss") << " Last:"<< QDateTime::fromMSecsSinceEpoch(m_lastSeen).toString("hhMMss");
    //return ((QDateTime::currentMSecsSinceEpoch() - m_lastSeen) > 5000);
    return m_devLost;
}

void BMS_SVIDevice::feedData(quint8 id, quint16 msg, QByteArray data){
    if(id == SVI_ID){
        m_currentTime = QDateTime::currentMSecsSinceEpoch();
        QDataStream ds(&data,QIODevice::ReadOnly);
        ds.setByteOrder(QDataStream::LittleEndian);
        qint16 v;
        ds >> v;
        m_aux1 = v;
        ds >> v;
        m_aux2 = v;

        ds >> v;
        if((v>-5) && (v < 5)){
            v = 0;
        }
        m_stackCurrent = v;
        ds >> v;
        m_stackVoltage = v;



        m_stackCurrent = m_simAmpere==0?m_stackCurrent:m_simAmpere;
        m_stackVoltage = m_simVolt==0?m_stackVoltage:m_simVolt;
        this->validAlarm();
        this->calculateState();
    }
}

void BMS_SVIDevice::calculateState()
{
    // calculate SOC
    float sv = (float)m_stackVoltage/10.;
    float sa = (float)(m_stackCurrent)/10.;
    float dt = (m_currentTime - m_lastSeen)/1000.;
    m_lastSeen = m_currentTime;
    //qDebug()<<Q_FUNC_INFO<<"Last seen:"<<m_lastSeen;

    float soc_new;

    soc_new = 100*(sa * dt / 3600./((m_soh/100.)*m_capacity)) + m_soc;
    if(soc_new >100) soc_new = 100;
    if(soc_new < 0) soc_new = 0;

    if((sa < 0.5) && (sa > -0.5)){
        return;
    }
    m_soc = soc_new;
//    m_soc = m_simSOC==0?soc_new:m_simSOC;


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
    int sv = m_simVolt==0?m_stackVoltage:m_simVolt;
    m_SVRule.valid(sv);

    int soc = m_simSOC==0?m_soc:m_simSOC;

    m_SOCRule.valid(soc);

    return;
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

    // soc


}

int BMS_SVIDevice::ovAlarm()
{
    return m_SVRule.ovAlarm();

    if(!m_ovAlarmEn) return 0;
    bool set =false;
    bool reset = false;
    int ret = 0;
    if(m_ovAlarmSetCntr > m_holdSecAlarm){
        set = true;
    }
    else if(m_ovAlarmClrCntr > m_holdSecAlarm){
        reset = true;
    }
    if(m_ovAlarmIsSet){
        if(set){
            ret = 0;
        }
        else if(reset){
            ret = 2;
            m_ovAlarmIsSet = false;
            //m_ovAlarmSetCntr = 0;
        }
        else{
            ret = 0;
        }
    }
    else{
        if(set){
            ret = 1;
            m_ovAlarmIsSet = true;
            //m_ovAlarmClrCntr = 0;
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
    return m_SVRule.ovWarning();
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
    return m_SVRule.uvAlarm();
    if(!m_uvAlarmEn) return 0;
    bool set =false;
    bool reset = false;
    int ret = 0;
    if(m_uvAlarmSetCntr > m_holdSecAlarm){
        set = true;
    }
    else if(m_uvAlarmClrCntr > m_holdSecAlarm){
        reset = true;
    }
    if(m_uvAlarmIsSet){
        if(set){
            ret = 0;
        }
        else if(reset){
            ret = 2;
            m_uvAlarmIsSet = false;
        }
        else{
            ret = 0;
        }
    }
    else{
        if(set){
            ret = 1;
            m_uvAlarmIsSet = true;
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
    return m_SVRule.uvWarning();
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

int BMS_SVIDevice::socWarning()
{
    return m_SOCRule.uvWarning();
}

int BMS_SVIDevice::socAlarm()
{
    return m_SOCRule.uvAlarm();
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

    m_SVRule.reset();
    m_SOCRule.reset();
}

void BMS_SVIDevice::simData()
{
    m_currentTime = QDateTime::currentMSecsSinceEpoch();
    this->validAlarm();
    this->calculateState();
}

QDataStream& operator << (QDataStream &out, const BMS_SVIDevice *svi)
{
//    int sv = svi->m_stackVoltage + svi->m_simVolt;
//    int sa = svi->m_stackCurrent + svi->m_simAmpere;
//    float soc = svi->m_soc + svi->m_simSOC;
    int sv = svi->m_simVolt==0? svi->m_stackVoltage:svi->m_simVolt;
    int sa = svi->m_simAmpere==0?svi->m_stackCurrent:svi->m_simAmpere;
    float soc = svi->m_soc;
    //qDebug()<<"Feed out :"<<svi->m_lastSeen;
    out << svi->m_devLost;
    out << sv;
    out << sa;
    out << (svi->m_soh);
    out << soc;
    out << svi->m_sohTrack;
    out << svi->m_SVRule.alarm_high.Enabled;
    out << svi->m_SVRule.warning_high.Enabled;
    out << svi->m_SVRule.alarm_low.Enabled;
    out << svi->m_SVRule.warning_low.Enabled;
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
    in >> svi->m_devLost;
    in >> svi->m_stackVoltage;
    in >> svi->m_stackCurrent;
    in >> svi->m_soh;
    in >> svi->m_soc;
    in >> svi->m_sohTrack;
    in >> svi->m_SVRule.alarm_high.Enabled;
    in >> svi->m_SVRule.warning_high.Enabled;
    in >> svi->m_SVRule.alarm_low.Enabled;
    in >> svi->m_SVRule.warning_low.Enabled;
//    in >> svi->m_ovAlarmIsSet;
//    in >> svi->m_ovWarningIsSet;
//    in >> svi->m_uvAlarmIsSet;
//    in >> svi->m_uvWarningIsSet;

    //qDebug()<<"Feed in:"<<svi->m_lastSeen;
    return in;
}

/*
 * valid if board lost communictaion, shall called by controller
 */
void BMS_SVIDevice::valid(int interval_seconds)
{
    long long ct = QDateTime::currentMSecsSinceEpoch();
    if((ct - m_lastSeen) > (interval_seconds*1000)){
        m_devLost = true;
    }
    else{
        m_devLost = false;
    }
}
