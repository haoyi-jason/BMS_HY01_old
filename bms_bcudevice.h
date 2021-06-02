#ifndef BMS_BCUDEVICE_H
#define BMS_BCUDEVICE_H

#include <QObject>
#include "bms_def.h"



class BMS_BCUDevice : public QObject
{
    Q_OBJECT
public:
    explicit BMS_BCUDevice(QObject *parent = nullptr);
    void add_digital_input(int n, bool inverted=false);
    void add_digital_output(int n, bool inverted=false);
    void add_analog_input(int n);
    void add_voltage_source(int n);
    void add_pwm_in(int n);
    int lastSeen();
    QByteArray getDigitalInput();
    QByteArray getDigitalOutput();
    QList<int> getWorkingCurrent();
    CAN_Packet *setDigitalOut(int id, int state);

    CAN_Packet *setVoltageSource(int id, int currentMa, bool enable = true);
    CAN_Packet *setADCRawLow(int id, int value);
    CAN_Packet *setADCRawHigh(int id, int value);

    CAN_Packet *setADCEngLow(int id, float value);
    CAN_Packet *setADCEngHigh(int id, float value);
    CAN_Packet *saveParam();
    void generatePacket();
    void feedData(quint8 id, quint16 msg, QByteArray data);
    int isActionPending();
   // QByteArray genPacket(PendingAction action);
    friend QDataStream& operator << (QDataStream &out, const BMS_BCUDevice *dev);
    friend QDataStream& operator >> (QDataStream &in, BMS_BCUDevice *dev);
    CAN_Packet* ActionPending();


signals:

public slots:

private:
    QList<HW_IOChannel*> m_digitalInput;
    QList<HW_IOChannel*> m_digitalOutput;
    QList<HW_IOChannel*> m_analogInput;
    QList<HW_IOChannel*> m_pwmInput;
    QList<HW_IOChannel*> m_voltageSource;
    long long m_lastSeen;
    QList<CAN_Packet*> m_pendingAction;
};

#endif // BMS_BCUDEVICE_H
