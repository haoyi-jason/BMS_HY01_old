#ifndef BMS_DEF_H
#define BMS_DEF_H

#include <QObject>
#include <QtCore>
#include <QtNetwork>
#include <QDateTime>
//#include "bms_bcudevice.h"

class QTcpSocket;
class CANBUSDevice;
class MODBUSDevice;
class BMS_BCUDevice;
class BMS_System;

namespace bms {
#define GROUP_OF(x)     (x >> 5)
#define ID_OF(x)        (x & 0x1F)
#define GROUP(x)        (x<< 5)
#define ID(x)           (x)
#define SVI_ID          0x1F
#define BCU_ID          0x01
    const int MAX_CELLS = 12;
    const int MAX_NTC = 5;

    enum AlarmID{ // low 16-bit for warning, high for alarm
        STACK_OV,
        STACK_UV,
        CELL_OV,
        CELL_UV,
        CELL_OT,
        CELL_UT,
        BMU_LOST,
        SVI_LOST,
        STACK_OT,
        STACK_UT,
        BCU_LOST,
        SOC_LOW_W,
        REV2,
        REV3,
        REV4,
        REV5,
        STACK_OVA,
        STACK_UVA,
        CELL_OVA,
        CELL_UVA,
        CELL_OTA,
        CELL_UTA,
        SOC_LOW_A
    };
}


class HW_IOChannel{
public:
    explicit HW_IOChannel(int initValue = 0, bool inverted = false){
        m_value = initValue;
        m_valueToWrite = initValue;
        //m_valueReadBack = initValue;
    }

    int value(){return m_value;}
    void value(int v){m_value = v;}
    void writeValue(int value){m_valueToWrite = value;}
    int writeValue(){return m_valueToWrite;}
    //void setReadback(int value){m_valueReadBack = value;}
    bool valid(){return (m_valueToWrite == m_value);}
    bool valid_write(){return !(m_valueToWrite == m_value);}
    void limit(int v){m_limit = v;}
    int limit(){return m_limit;}
private:
    int m_valueToWrite;
    //int m_valueReadBack;
    int m_value;
    int m_limit;
};

class CAN_Packet{
public:
    quint16 Command;
    QByteArray data;
    bool readFrame= false;
    bool writeOK = false;
    bool remote = false;
};

class _EventItem{
public:
    int setValue;
    int resetValue;
    int holdTime;
    int activeTime;
};

class ValueDef{
public:
    qint32 value_set = 0;
    qint32 value_reset = 0;
    qint32 holdTime = 0;
    qint32 activeCount = 0;
    //bool activeated=false;
    bool valid(){return (activeCount > holdTime);}
};

class EventDef{
public:
    ValueDef high_thres;
    ValueDef low_thres;
    bool high_active = false;
    bool low_active = false;
    bool autoreset = false;
    QString name = "";
};

class _EventCriteria
{

public:
    ValueDef *alarmHigh = nullptr;
    ValueDef *alarmLow = nullptr;
    ValueDef *warningHigh = nullptr;
    ValueDef *warningLow = nullptr;
    void validEvents(qint32 tpv){
        if(alarmHigh != nullptr){
            if(tpv > alarmHigh->value_set){
                alarmHigh->activeCount++;
            }
            else{
                alarmHigh->activeCount = 0;
            }
        }

        if(warningHigh != nullptr){
            if(tpv > warningHigh->value_set){
                warningHigh->activeCount++;
            }
            else{
                warningHigh->activeCount = 0;
            }
        }

        if(alarmLow != nullptr){
            if(tpv < alarmLow->value_set){
                alarmLow->activeCount++;
            }
            else{
                alarmLow->activeCount = 0;
            }
        }
        if(warningLow != nullptr){
            if(tpv < warningLow->value_set){
                warningLow->activeCount++;
            }
            else{
                warningLow->activeCount = 0;
            }
        }
    }

    //qint32 tpv = 0; // current value
};

class _Events{
public:
    int eventType;
    int startTime;
    QString eventDescription;
};



/*
 *  stack information of a BMS
 *  each stack contains n battery and at most 1 stack voltage/current module,
 *  32-devices per stack with 8-bit CANBUS device address mapping, MSB 3-bits
 *  for group, LSB 5-bit for address
 *  1. stack voltage/current module config at fixed address 0x01
 *  2. battery (at most 12-cells) start from 2 to 31, 29 batteries most
 *  3. address 0x0 (group 0/ id 0) is for broadcasting.
 */

class BMS_Event:public QObject{
    Q_OBJECT
public:
    explicit BMS_Event(QObject *parent = nullptr){}
    friend QTextStream &operator << (QTextStream &out,const BMS_Event *event){
        out << QString("%1;").arg(event->DateString);
        out << QString("%1;").arg(event->TimeString);
        out << QString("%1;").arg(event->EventName);
        out << QString("%1;").arg(event->Level);
        out << QString("%1;").arg(event->State);
        out << QString("%1\n").arg(event->Information);
    }

    bool parse(QString msg){
        QStringList sl = msg.split(";");
        if(sl.size() == 6){
            DateString = sl[0];
            TimeString = sl[1];
            EventName = sl[2];
            Level = sl[3];
            State = (sl[4]);
            Information = sl[5];
        }
        else{
            return false;
        }
    }



public:
    QString DateString, TimeString;
    QString EventName;
    QString Level;
    QString State;
    QString Information;
};


class RemoteSystem{
public:
    QString connection="";
    QTcpSocket *socket = nullptr;
    BMS_System *system = nullptr;
    bool configReady = false; // should be false
    QByteArray data;
    QString alias;
    QString logPath;
    int port;
    bool autoConnect;
    bool enableLog=false;
    int logDays = -1;
    int logRecords = 1000;
    QDateTime lastSeen;
    void setDigitalOut(int id, int value);

    void setVoltageSource(int id, int value);

    void openSerialPort(QString name, int baudrate, int parity, int stopBits);

    void closeSerialPort(QString name);

    void writeSerialPort(QString name, QString data);
    void writeCommand(QString command);

    void logData(QByteArray b);
};


class BMS_Criteria_Pair{
public:
    explicit BMS_Criteria_Pair(){
        setMask.clear();
        clrMask.clear();
    }
    void mask_reset(){
        setMask.clear();
        clrMask.clear();
    }
    void mask_feed(){
        setMask.append(mask_set);
        clrMask.append(mask_clr);
        mask_set = mask_clr = 0;
        if(setMask.size() > Size)
            setMask.removeFirst();
        if(clrMask.size() > Size)
            clrMask.removeFirst();
    }
    void setMaskBit(int b){
        mask_set |= (1 << b);
    }
    void clrMaskBit(int b){
        mask_clr |= (1 << b);
    }

    quint16 State(quint16 *set, quint16 *clr){
        if(setMask.size() < Size) return 0x0;
        if(clrMask.size() < Size) return 0x0;
        ushort sm = 0xffff; //set mask
        ushort cm = 0xffff; // clr mask
        foreach(quint16 v, setMask){
            sm &= v;
        }
        foreach(quint16 v, clrMask){
            cm &= v;
        }
        *set = sm;
        *clr = cm;
        enabledMaskNew = sm ^ enabledMask;
        return enabledMaskNew;
    }
    void Handled(quint16 mask){
        enabledMask = mask;
    }

    QList<quint16> setMask, clrMask;
    quint16 mask_set = 0, mask_clr=0;
    int set=0,clr=0;
    bool enable = false;
    quint8 Size = 5;
    quint16 enabledMask = 0x0;
    quint16 enabledMaskNew = 0x0;
};

class BMS_Criteria_Rule{
public:
    void valid(QList<short> v){
        qint32 max = INT_MIN, min=INT_MAX;
        int total = 0;
        int maxid = -1, minid = -1;
        for(int i=0;i<v.size();i++){
            if(v[i] > max){
                max = v[i];
                maxid = i;
            }
            if(v[i] < min){
                min = v[i];
                minid = i;
            }
            total += v[i];
            if(v[i] > alarm_high.set){
                alarm_high.setMaskBit(i);
            }
            if( v[i] < alarm_high.clr){
                alarm_high.clrMaskBit(i);
            }
            if(v[i] > warning_high.set){
                warning_high.setMaskBit(i);
            }
            if(v[i] < warning_high.clr){
                warning_high.clrMaskBit(i);
            }

            if(v[i] < alarm_low.set){
                alarm_low.setMaskBit(i);
            }
            if(v[i] > alarm_low.clr){
                alarm_low.clrMaskBit(i);
            }
            if(v[i] < warning_low.set){
                warning_low.setMaskBit(i);
            }
            if( v[i] > warning_low.clr){
                warning_low.clrMaskBit(i);
            }
        }
        Max = max; Min = min,Total=total;
        ID_Max = maxid;ID_Min = minid;
        alarm_high.mask_feed();
        alarm_low.mask_feed();;
        warning_high.mask_feed();
        warning_low.mask_feed();
    }
    void reset(){
        alarm_high.mask_reset();
        alarm_high.enabledMask = 0x0;
        alarm_low.mask_reset();
        alarm_low.enabledMask = 0x0;
        warning_high.mask_reset();
        warning_high.enabledMask = 0x0;
        warning_low.mask_reset();
        warning_low.enabledMask = 0x0;
    }

    quint16 ovStateWarning(quint16 *set, quint16 *clr){
        return warning_high.State(set,clr);
    }
    quint16 ovStateAlarm(quint16 *set, quint16 *clr){
        return alarm_high.State(set,clr);
    }
    quint16 udStateWarning(quint16 *set, quint16 *clr){
        return warning_low.State(set,clr);
    }
    quint16 udStateAlarm(quint16 *set, quint16 *clr){
        return alarm_low.State(set,clr);
    }
    BMS_Criteria_Pair alarm_high, alarm_low;
    BMS_Criteria_Pair warning_high,warning_low;
    qint32 Max=0,Min=0,Total=0;
    qint32 ID_Max=-1,ID_Min=-1;
};

class BMS_Criteria_Single{
public:
    void reset(){
        setCount = clrCount = 0;
    }
    void Set()
    {
        setCount++;
        clrCount = 0;
    }
    void Clr(){
        clrCount++;
        setCount = 0;
    }

    quint8 State(){
        quint8 ret = 0;
        if(setCount > Size){
            ret = Enabled?0:1;
            Enabled = true;
        }
        else if(clrCount > Size){
            ret = Enabled?2:0;
            Enabled = false;
        }
        return ret;
    }

    void Handled(quint8 v){

    }

    quint8 setCount=0, clrCount=0;
    quint8 Size = 5;
    bool Enable = false;
    bool Enabled = false;
    int set = 0, clr = 0;
};

class BMS_CriteriaRule_Single{
public:
    void valid(int v){
        if(v > alarm_high.set){
            alarm_high.Set();
        }
        else if(v < alarm_high.clr){
            alarm_high.Clr();
        }
        else {
            alarm_high.reset();
        }

        if(v > warning_high.set)
            warning_high.Set();
        else  if(v < warning_high.clr)
            warning_high.Clr();
        else
            warning_high.reset();

        if(v < alarm_low.set)
            alarm_low.Set();
        else if(v > alarm_low.clr)
            alarm_low.Clr();
        else
            alarm_low.reset();

        if(v < warning_low.set)
            warning_low.Set();
        else if(v > warning_low.clr)
            warning_low.Clr();
        else
            warning_low.reset();
    }
    void reset(){
        alarm_high.reset();alarm_low.reset();
        warning_high.reset();warning_low.reset();
        alarm_high.Enabled = false;
        alarm_low.Enabled = false;
        warning_high.Enabled = false;
        warning_low.Enabled = false;
    }

    quint8 ovWarning(){
        return warning_high.State();
    }
    quint8 ovAlarm(){
        return alarm_high.State();
    }
    quint8 uvWarning(){
        return warning_low.State();
    }
    quint8 uvAlarm(){
        return alarm_low.State();
    }

    BMS_Criteria_Single alarm_high,alarm_low;
    BMS_Criteria_Single warning_high, warning_low;

};



#endif // BMS_DEF_H
