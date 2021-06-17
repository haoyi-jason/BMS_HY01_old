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
    ushort getCellVoltage(int index);
    ushort getPackTemperature(int index);

    ushort cellVoltage(int index);
    QList<ushort> cellVoltages();
    void cellVoltage(int id, ushort x);
    void cellVoltages(QList<ushort>x);

    ushort packTemperature(int index);
    QList<ushort> packTemperatures();
    void packTemperature(int id, ushort x);
    void packTemperatures(QList<ushort>x);

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
    ushort minCellVoltage();
    ushort maxCellVoltage();
    ushort totalVoltage();
    ushort minPackTemp();
    ushort maxPackTemp();
    ushort cellVoltageDiff();
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
    bool deviceLost(){return ((QDateTime::currentMSecsSinceEpoch() - m_lastSeen) > 5000);}
    void resetValues();

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
    QList<ushort> m_cellVoltage;
    QList<ushort> m_packTemperature;
    QList<ushort> m_balancing;
    ushort m_balancingVoltage;
    ushort m_balancingHystersis;
    ushort m_balancingONSeconds;
    ushort m_balancingOFFSeconds;
    quint8 m_devid;
    QList<ushort> m_openWire;
    quint8 m_nofCell, m_nofNtc;
    ushort m_maxVoltage;
    ushort m_minVoltage;
    ushort m_maxTemperature;
    ushort m_minTemperature;
    ushort m_totalVoltage;
    long long m_lastSeen;
    QList<QByteArray> m_pendingAction;

    quint16 m_ov_set_ths = 0;
    quint16 m_ov_clr_ths = 0;
    quint16 m_uv_set_ths = 0;
    quint16 m_uv_clr_ths = 0;

    quint16 m_voltHighMask;
    quint16 m_voltLowMask;

    quint16 m_ot_set_ths;
    quint16 m_ot_clr_ths;
    quint16 m_ut_set_ths;
    quint16 m_ut_clr_ths;

    quint8 m_tempHighMask;
    quint8 m_tempLowMask;
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
};

#endif // BMS_BMUDEVICE_H
