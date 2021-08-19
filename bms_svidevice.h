#ifndef BMS_SVIDEVICE_H
#define BMS_SVIDEVICE_H

#include <QObject>
#include <QDateTime>
#include "bms_def.h"

class BMS_SVIDevice : public QObject
{
    Q_OBJECT
public:
    explicit BMS_SVIDevice(QObject *parent = nullptr);
    QByteArray data();
    int voltage();
    void voltage(int v){m_stackVoltage = v;}
    int current();
    void current(int v){m_stackCurrent  =v;}
    float soc(){return m_soc + m_simSOC;}
    void soc(float v){m_soc = v;}
    float soh(){return m_soh;}
    void soh(float v){m_soh = v;}
    void feedData(quint8 id, quint16 msg, QByteArray data);
    friend QDataStream& operator << (QDataStream &out, const BMS_SVIDevice *hvc);
    friend QDataStream& operator >> (QDataStream &in, BMS_SVIDevice *hvc);
    void clearAlarm();
    void voltAlarm(bool v){m_voltAlarm = v;}
    bool voltAlarm(){return m_voltAlarm;}
    void ampereAlarm(bool v){m_currentAlarm = v;}
    bool ampereAlarm(){return m_currentAlarm;}

    int lastSeen(){return QDateTime::currentMSecsSinceEpoch() - m_lastSeen;}
    bool deviceLost();
    void setSimVoltage(int v){m_simVolt = v;}
    void setSimAmpere(int v){m_simAmpere = v;}
    void setSimSOC(int v){m_simSOC = v;}
    float capacity(){return m_capacity;}
    void capacity(float v){m_capacity = v;}

    void calculateState();
    void setSOHTracking(bool state);

    void eventDuration(int v){m_holdSec = v;}

    void ovAlarmSet(int v){m_ovAlarmSet = v;}
    void ovAlarmClr(int v){m_ovAlarmClr = v;}
    void ovWarningSet(int v){m_ovWarningSet = v;}
    void ovWarningClr(int v){m_ovWarningClr = v;}

    void uvAlarmSet(int v){m_uvAlarmSet = v;}
    void uvAlarmClr(int v){m_uvAlarmClr = v;}
    void uvWarningSet(int v){m_uvWarningSet = v;}
    void uvWarningClr(int v){m_uvWarningClr = v;}

    void vWarningDuration(int v){m_holdSec = v;}
    void vAlarmDuration(int v){m_holdSecAlarm = v;}

    void ovAlarmEn(bool v){m_ovAlarmEn = v;}
    void ovWarningEn(bool v){m_ovWarningEn = v;}
    void uvAlarmEn(bool v){m_uvAlarmEn = v;}
    void uvWarningEn(bool v){m_uvWarningEn = v;}

    int ovAlarm();
    int uvAlarm();
    int ovWarning();
    int uvWarning();
    int socWarning();
    int socAlarm();

    bool isOvAlarm(){return m_SVRule.alarm_high.Enabled;}
    bool isOvWarning(){return m_SVRule.warning_high.Enabled;}
    bool isUvAlarm(){return m_SVRule.alarm_low.Enabled;}
    bool isUvWarning(){return m_SVRule.warning_low.Enabled;}
    bool isSOCAlarm(){return m_SOCRule.alarm_low.Enabled;}
    bool isSOCWarning(){return m_SOCRule.warning_low.Enabled;}

    void simData();

    void setSVAlarmHighPair(int set, int clr, int HoldCount = 5,bool enable = true){
        m_SVRule.alarm_high.set = set;
        m_SVRule.alarm_high.clr = clr;
        m_SVRule.alarm_high.Enable = enable;
        m_SVRule.alarm_high.Size = HoldCount;
    }

    void setSVAlarmLowPair(int set, int clr, int HoldCount = 5,bool enable = true){
        m_SVRule.alarm_low.set = set;
        m_SVRule.alarm_low.clr = clr;
        m_SVRule.alarm_low.Enable = enable;
        m_SVRule.alarm_low.Size = HoldCount;
    }
    void setSVWarningHighPair(int set,int clr, int HoldCount = 5,bool enable = true){
        m_SVRule.warning_high.set = set;
        m_SVRule.warning_high.clr = clr;
        m_SVRule.warning_high.Enable = enable;
        m_SVRule.warning_high.Size = HoldCount;
    }

    void setSVWarningLowPair(int set,int clr, int HoldCount = 5,bool enable = true){
        m_SVRule.warning_low.set = set;
        m_SVRule.warning_low.clr = clr;
        m_SVRule.warning_low.Enable = enable;
        m_SVRule.warning_low.Size = HoldCount;
    }

    void setSOCWarningLowPair(int set, int clr, int HoldCount = 5, bool enable = true){
        m_SOCRule.warning_low.set = set;
        m_SOCRule.warning_low.clr = clr;
        m_SOCRule.warning_low.Enable = enable;
        m_SOCRule.warning_low.Size = HoldCount;
    }

    void setSOCAlarmLowPair(int set, int clr, int HoldCount = 5, bool enable = true){
        m_SOCRule.alarm_low.set = set;
        m_SOCRule.alarm_low.clr = clr;
        m_SOCRule.alarm_low.Enable = enable;
        m_SOCRule.alarm_low.Size = HoldCount;
    }

    // keep info to prevent double recorred
    bool isLosted(){return m_isLosted;}
    void isLosted(bool v){m_isLosted = v;}

    bool isLost(){return m_devLost;}

    void valid(int interval_seconds = 5);



    static CAN_Packet *rawLow(CAN_Packet *p, quint8 gid,int channel, ushort raw){
        CAN_Packet *ret = new CAN_Packet;
        p->Command = 0x160 | ((GROUP(gid) | 0x1F)<<12);
        QDataStream ds(&p->data,QIODevice::WriteOnly);
        ds.setByteOrder(QDataStream::LittleEndian);
        quint8 b8;
        quint16 b16;
        b8 = channel;
        ds << b8;
        b8 = 0; // raw low
        ds << 8;
        ds >> raw;
        return ret;
    }

    static CAN_Packet *rawHigh(quint8 gid,int channel, ushort raw){
        CAN_Packet *p = new CAN_Packet;
        p->Command = 0x160 | ((GROUP(gid) | 0x1F)<<12);
        QDataStream ds(&p->data,QIODevice::WriteOnly);
        ds.setByteOrder(QDataStream::LittleEndian);
        quint8 b8;
        quint16 b16;
        b8 = channel;
        ds << b8;
        b8 = 1; // raw low
        ds << 8;
        ds >> raw;
        return p;
    }

    static CAN_Packet *engLow(quint8 gid,int channel, float eng){
        CAN_Packet *p = new CAN_Packet;
        p->Command = 0x160 | ((GROUP(gid) | 0x1F)<<12);
        QDataStream ds(&p->data,QIODevice::WriteOnly);
        ds.setByteOrder(QDataStream::LittleEndian);
        quint8 b8;
        b8 = channel;
        ds << b8;
        b8 = 2; // eng low
        ds << 8;
        ds << eng;
        return p;
    }

    static CAN_Packet *engHigh(quint8 gid,int channel, float eng){
        CAN_Packet *p = new CAN_Packet;
        p->Command = 0x160 | ((GROUP(gid) | 0x1F)<<12);
        QDataStream ds(&p->data,QIODevice::WriteOnly);
        ds.setByteOrder(QDataStream::LittleEndian);
        quint8 b8;
        b8 = channel;
        ds << b8;
        b8 = 3; // eng high
        ds << 8;
        ds << eng;
        return p;
    }

    static CAN_Packet *zeroCalibration(quint8 gid,int channel){
        CAN_Packet *p = new CAN_Packet;
        p->Command = 0x160 | ((GROUP(gid) | 0x1F)<<12);
        QDataStream ds(&p->data,QIODevice::WriteOnly);
        ds.setByteOrder(QDataStream::LittleEndian);
        quint8 b8;
        //quint16 b16;
        b8 = channel;
        ds << b8;
        b8 = 4; // zero calibration
        ds << 8;
        return p;
    }

    static CAN_Packet *bandCalibration(quint8 gid,int channel, float band){
        CAN_Packet *p = new CAN_Packet;
        p->Command = 0x160 | ((GROUP(gid) | 0x1F)<<12);
        QDataStream ds(&p->data,QIODevice::WriteOnly);
        ds.setByteOrder(QDataStream::LittleEndian);
        quint8 b8;
        b8 = channel;
        ds << b8;
        b8 = 5; // band calibration
        ds << 8;
        ds << band;
        return p;
    }

signals:
    void set_ov();
    void set_uv();
    void set_oc();
    void set_uc();
public slots:
    void validAlarm();
private:
    int m_aux1 = 0,m_aux2 = 0;
    int m_stackVoltage = 0;
    int m_stackCurrent=0;
    QList<ushort> m_auxInputs;
    bool m_voltAlarm = false;
    bool m_currentAlarm = false;
    long long m_lastSeen,m_currentTime;
    float m_soc = 100, m_soh = 100;
    int m_simVolt = 0, m_simAmpere = 0;
    int m_simSOC = 0;
    float m_capacity = 50; // Battery's capacity,AH
    bool m_sohTrack = false;
    double m_sohAccum = 0;

    int m_ovAlarmSet = 0;
    int m_ovAlarmClr = 0;
    int m_ovWarningSet = 0;
    int m_ovWarningClr = 0;

    int m_uvAlarmSet = 0;
    int m_uvAlarmClr = 0;
    int m_uvWarningSet = 0;
    int m_uvWarningClr = 0;

    int m_ocAlarmSet = 0;
    int m_ocAlarmClr = 0;
    int m_ocWarningSet = 0;
    int m_ocWarningClr = 0;

    int m_ucAlarmSet = 0;
    int m_ucAlarmClr = 0;
    int m_ucWarningSet = 0;
    int m_ucWarningClr = 0;

    bool m_ovAlarmEn = false;
    bool m_ovWarningEn = false;
    bool m_uvAlarmEn = false;
    bool m_uvWarningEn = false;

    bool m_ocAlarmEn = false;
    bool m_ocWarningEn = false;
    bool m_ucAlarmEn = false;
    bool m_ucWarningEn = false;

    int m_ovAlarmSetCntr = 0;
    int m_ovAlarmClrCntr = 0;
    int m_ovWarningSetCntr = 0;
    int m_ovWarningClrCntr = 0;

    int m_uvAlarmSetCntr = 0;
    int m_uvAlarmClrCntr = 0;
    int m_uvWarningSetCntr = 0;
    int m_uvWarningClrCntr = 0;

    int m_ocAlarmSetCntr = 0;
    int m_ocAlarmClrCntr = 0;
    int m_ocWarningSetCntr = 0;
    int m_ocWarningClrCntr = 0;

    int m_ucAlarmSetCntr = 0;
    int m_ucAlarmClrCntr = 0;
    int m_ucWarningSetCntr = 0;
    int m_ucWarningClrCntr = 0;

    bool m_ovAlarmIsSet = false;
    bool m_uvAlarmIsSet = false;
    bool m_ovWarningIsSet = false;
    bool m_uvWarningIsSet = false;
    int m_holdSec = 5;
    int m_holdSecAlarm = 5;

    BMS_CriteriaRule_Single m_SVRule;
    BMS_CriteriaRule_Single m_SOCRule;

    bool m_isLosted = false;
    bool m_devLost = false;

//    BMS_Criteria_Rule m_STrules;
};

#endif // BMS_SVIDEVICE_H
