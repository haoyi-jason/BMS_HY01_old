#ifndef BMS_BMUDEVICE_H
#define BMS_BMUDEVICE_H

#include <QObject>
#include "bms_def.h"

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
    void dummyData(ushort vbase=3400, ushort vgap=100, ushort tbase = 250, ushort tgap = 50);
    QByteArray data();
    friend QDataStream& operator << (QDataStream &out, const BMS_BMUDevice *bat);
    friend QDataStream& operator >> (QDataStream &in, BMS_BMUDevice *bat);
    QByteArray ActionPending();
    QList<ushort> openWire(){return m_openWire;}
    ushort openWire(int index);

    CAN_Packet *setBalancing(quint16 bv, quint8 bh, quint8 be, quint16 on, quint16 off);
signals:

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
    ushort m_voltHighThreshold;
    ushort m_voltLowThreshold;
    ushort m_voltHighMask;
    ushort m_voltLowMask;
    ushort m_tempHighThreshold;
    ushort m_tempLowThreshold;
    quint8 m_tempHighMask;
    quint8 m_tempLowMask;

};

#endif // BMS_BMUDEVICE_H
