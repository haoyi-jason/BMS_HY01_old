#ifndef BMS_BMUDEVICE_H
#define BMS_BMUDEVICE_H

#include <QObject>
#include "bms_def.h"

class BMS_BCUEmgData{
public:
    quint8 cell_id;
    quint8 volt_diff[9];
};



class BMS_BMUDevice : public QObject
{
    Q_OBJECT
public:
    explicit BMS_BMUDevice(QObject *parent = nullptr);
    BMS_BMUDevice(int nofCells, int nofTemp,QObject *parent = nullptr);
    short getCellVoltage(int index);
    short getPackTemperature(int index);

    short cellVoltage(int index);
    QList<short> cellVoltages();
    void cellVoltage(int id, short x);
    void cellVoltages(QList<short>x);

    short packTemperature(int index);
    QList<short> packTemperatures();
    void packTemperature(int id, short x);
    void packTemperatures(QList<short>x);

    ushort balancing(int index);
    QList<ushort> balancing();
    void balancing(QList<ushort>x);
    void balancing(int index, ushort x);
    void balancingVoltage(ushort value);
    void balancingHystersis(ushort value);
    void balancingONTime(ushort value);
    void balancingOFFTime(ushort value);
    quint8 deviceID();

    void deviceID(quint8 value);
    quint8 cellCount();
    quint8 ntcCount();
    short minCellVoltage();
    short maxCellVoltage();
    qint32 totalVoltage();
    short minPackTemp();
    short maxPackTemp();
    ushort maxCID();
    ushort minCID();
    ushort maxTID();
    ushort minTID();
    short cellVoltageDiff();
    int lastSeen();
    void feedData(uint8_t id, uint16_t msg, QByteArray data);
    void simData(ushort vbase=3400, ushort vgap=100, ushort tbase = 250, ushort tgap = 50);
    QByteArray data();
    friend QDataStream& operator << (QDataStream &out, const BMS_BMUDevice *bat);
    friend QDataStream& operator >> (QDataStream &in, BMS_BMUDevice *bat);
    QByteArray ActionPending();
    QList<ushort> openWire(){return m_openWire;}
    ushort openWire(int index);

    CAN_Packet *setBalancing(quint16 bv, quint8 bh, quint8 be, quint16 on, quint16 off);
    void clearAlarm();
    bool alarm();
    void alarm(bool v);

    quint16 ovState(quint16 *set, quint16 *clr);
    quint16 uvState(quint16 *set, quint16 *clr);
    quint16 otState(quint16 *set, quint16 *clr);
    quint16 utState(quint16 *set, quint16 *clr);

    bool isOV();
    bool isUV();
    bool isOT();
    bool isUT();

    bool isOVA();
    bool isUVA();
    bool isOTA();
    bool isUTA();

    void setSimVolt(quint8 id, quint8 cell, ushort shift);
    void setSimTemp(quint8 id, quint8 tid, ushort shift);
    void ov_set(ushort v){m_ov_set_ths = v;}
    void ov_clr(ushort v){m_ov_clr_ths = v;}
    void uv_set(ushort v){m_uv_set_ths = v;}
    void uv_clr(ushort v){m_uv_clr_ths = v;}
    void ot_set(ushort v){m_ot_set_ths = v;}
    void ot_clr(ushort v){m_ot_clr_ths = v;}
    void ut_set(ushort v){m_ut_set_ths = v;}
    void ut_clr(ushort v){m_ut_clr_ths = v;}
    void duration(ushort v){m_holdSec = v;}
    ushort balancingBit(){return m_balancingBit;}
    ushort openwireBit(){return m_openWireBit;}
    void validAlarmstate();
    void ovSetMask(quint16 v){m_ovSetMask = v;}
    quint16 ovSetMask(){return m_ovSetMask;}
    void uvSetMask(quint16 v){m_uvSetMask = v;}
    quint16 uvSetMask(){return m_uvSetMask;}
    void otSetMask(quint16 v){m_otSetMask = v;}
    quint16 otSetMask(){return m_otSetMask;}
    void utSetMask(quint16 v){m_utSetMask = v;}
    quint16 utSetMask(){return m_utSetMask;}
    bool deviceLost(){
        //qDebug()<<"BMU:NOW:"<<QDateTime::currentDateTime().toString("hhMMss") << " Last:"<< QDateTime::fromMSecsSinceEpoch(m_lastSeen).toString("hhMMss");
//        if((QDateTime::currentMSecsSinceEpoch() - m_lastSeen) > 5000){
//            m_devLost = true;
//        }
//        else{
//            m_devLost = false;
//        }
        return m_devLost;
        //return m_devLost;
    }
    bool isLost(){return m_devLost;}
    void resetValues();
    void valid(int interval_seconds = 5);

    void setCVAlarmHighPair(int set, int clr, int HoldCount = 5,bool enable = true){
        m_CVrules.alarm_high.set = set;
        m_CVrules.alarm_high.clr = clr;
        m_CVrules.alarm_high.enable = enable;
        m_CVrules.alarm_high.Size = HoldCount;
    }
    void setCVAlarmLowPair(int set, int clr,int HoldCount = 5,bool enable = true){
        m_CVrules.alarm_low.set = set;
        m_CVrules.alarm_low.clr = clr;
        m_CVrules.alarm_low.enable = enable;
        m_CVrules.alarm_low.Size = HoldCount;
    }
    void setCVWarningHighPair(int set, int clr,int HoldCount = 5,bool enable = true){
        m_CVrules.warning_high.set = set;
        m_CVrules.warning_high.clr = clr;
        m_CVrules.warning_high.enable = enable;
        m_CVrules.warning_high.Size = HoldCount;
    }
    void setCVWarningLowPair(int set, int clr,int HoldCount = 5,bool enable = true){
        m_CVrules.warning_low.set = set;
        m_CVrules.warning_low.clr = clr;
        m_CVrules.warning_low.enable = enable;
        m_CVrules.warning_low.Size = HoldCount;
    }

    void setCTAlarmHighPair(int set, int clr,int HoldCount = 5,bool enable = true){
        m_CTrules.alarm_high.set = set;
        m_CTrules.alarm_high.clr = clr;
        m_CTrules.alarm_high.enable = enable;
        m_CTrules.alarm_high.Size = HoldCount;
    }
    void setCTAlarmLowPair(int set, int clr,int HoldCount = 5,bool enable = true){
        m_CTrules.alarm_low.set = set;
        m_CTrules.alarm_low.clr = clr;
        m_CTrules.alarm_low.enable = enable;
        m_CTrules.alarm_low.Size = HoldCount;
    }
    void setCTWarningHighPair(int set, int clr,int HoldCount = 5,bool enable = true){
        m_CTrules.warning_high.set = set;
        m_CTrules.warning_high.clr = clr;
        m_CTrules.warning_high.enable = enable;
        m_CTrules.warning_high.Size = HoldCount;
    }
    void setCTWarningLowPair(int set, int clr,int HoldCount = 5,bool enable = true){
        m_CTrules.warning_low.set = set;
        m_CTrules.warning_low.clr = clr;
        m_CTrules.warning_low.enable = enable;
        m_CTrules.warning_low.Size = HoldCount;
    }

    quint16 cvWarningOVState(quint16 *set, quint16 *clr);
    quint16 cvWarningUVState(quint16 *set, quint16 *clr);
    quint16 cvAlarmOVState(quint16 *set, quint16 *clr);
    quint16 cvAlarmUVState(quint16 *set, quint16 *clr);
    quint16 ctWarningOVState(quint16 *set, quint16 *clr);
    quint16 ctWarningUVState(quint16 *set, quint16 *clr);
    quint16 ctAlarmOVState(quint16 *set, quint16 *clr);
    quint16 ctAlarmUVState(quint16 *set, quint16 *clr);

    void cvWarningOVHandled(quint16 mask){m_CVrules.warning_high.Handled(mask);}
    void ctWarningOVHandled(quint16 mask){m_CTrules.warning_high.Handled(mask);}
    void cvWarningUVHandled(quint16 mask){m_CVrules.warning_low.Handled(mask);}
    void ctWarningUVHandled(quint16 mask){m_CTrules.warning_low.Handled(mask);}
    void cvAlarmOVHandled(quint16 mask){m_CVrules.alarm_high.Handled(mask);}
    void ctAlarmOVHandled(quint16 mask){m_CTrules.alarm_high.Handled(mask);}
    void cvAlarmUVHandled(quint16 mask){m_CVrules.alarm_low.Handled(mask);}
    void ctAlarmUVHandled(quint16 mask){m_CTrules.alarm_low.Handled(mask);}

    quint16 cvWarningOVMask(){return m_CVrules.warning_high.enabledMask;}
    quint16 cvWarningUVMask(){return m_CVrules.warning_low.enabledMask;}
    quint16 cvAlarmOVMask(){return m_CVrules.alarm_high.enabledMask;}
    quint16 cvAlarmUVMask(){return m_CVrules.alarm_low.enabledMask;}

    quint16 ctWarningOVMask(){return m_CTrules.warning_high.enabledMask;}
    quint16 ctWarningUVMask(){return m_CTrules.warning_low.enabledMask;}
    quint16 ctAlarmOVMask(){return m_CTrules.alarm_high.enabledMask;}
    quint16 ctAlarmUVMask(){return m_CTrules.alarm_low.enabledMask;}

    bool isLosted(){return m_isLosted;}
    void isLosted(bool v){m_isLosted = v;}
signals:
    void set_ov(quint16);
    void set_uv(quint16);
    void set_ot(quint16);
    void set_ut(quint16);

    void clear_ov();
    void clear_uv();
    void clear_ot();
    void clear_ut();

    void emgData(quint8 did,quint8 cid, quint16 baseVolt, QByteArray voltQueue);


public slots:
private:
    QList<short> m_cellVoltage;
    QList<short> m_packTemperature;
    QList<ushort> m_balancing;
    ushort m_balancingVoltage=0;
    ushort m_balancingHystersis=0;
    ushort m_balancingONSeconds=0;
    ushort m_balancingOFFSeconds=0;
    quint8 m_devid=0;
    QList<ushort> m_openWire;
    quint8 m_nofCell=0, m_nofNtc=0;
    ushort m_maxVoltage=0;
    ushort m_minVoltage=0;
    ushort m_maxTemperature=0;
    ushort m_minTemperature=0;
    qint32 m_totalVoltage=0;
    long long m_lastSeen;
    QList<QByteArray> m_pendingAction;

    quint16 m_ov_set_ths = 0;
    quint16 m_ov_clr_ths = 0;
    quint16 m_uv_set_ths = 0;
    quint16 m_uv_clr_ths = 0;

    quint16 m_voltHighMask=0;
    quint16 m_voltLowMask=0;

    qint16 m_ot_set_ths=0;
    qint16 m_ot_clr_ths=0;
    qint16 m_ut_set_ths=0;
    qint16 m_ut_clr_ths=0;

    quint8 m_tempHighMask=0;
    quint8 m_tempLowMask=0;
    QList<BMS_BCUEmgData> m_emgVolt;
    QList<quint16> m_ovMask; // over voltage mask, record on every packet received, check by top system use ored arith
    QList<quint16> m_ovClrMask;
    QList<quint16> m_uvMask;
    QList<quint16> m_uvClrMask;
    QList<quint16> m_otMask;
    QList<quint16> m_otClrMask;
    QList<quint16> m_utMask;
    QList<quint16> m_utClrMask;
    int m_holdSec = 5; // to do : set by config
    bool m_alarm = false;
    qint8 m_VoltState = 0, m_TempState = 0;
    quint16 m_ovSetMask = 0x0000;
    quint16 m_uvSetMask = 0x0000;
    quint16 m_otSetMask = 0x0000;
    quint16 m_utSetMask = 0x0000;

    bool m_ovAlarm=false, m_uvAlarm=false, m_otAlarm=false, m_utAlarm=false;

    QList<ushort> m_simVoltBase;
    QList<ushort> m_simTempBase;

    quint16 m_balancingBit = 0x0;
    quint16 m_openWireBit = 0x0;
    bool m_devLost = false;
    BMS_Criteria_Rule m_CVrules,m_CTrules;
    bool m_isLosted = false;
};

#endif // BMS_BMUDEVICE_H
