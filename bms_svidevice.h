#ifndef BMS_SVIDEVICE_H
#define BMS_SVIDEVICE_H

#include <QObject>
#include <QDateTime>
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
    int voltageAlarm();
    int currentAlarm();
    void voltAlarm(bool v){m_voltAlarm = v;}
    bool voltAlarm(){return m_voltAlarm;}
    void ampereAlarm(bool v){m_currentAlarm = v;}
    bool ampereAlarm(){return m_currentAlarm;}
    int lastSeen(){return QDateTime::currentMSecsSinceEpoch() - m_lastSeen;}
    bool deviceLost(){return ((QDateTime::currentMSecsSinceEpoch() - m_lastSeen) > 5000);}
    void setSimVoltage(int v){m_simVolt = v;}
    void setSimAmpere(int v){m_simAmpere = v;}
    void setSimSOC(int v){m_simSOC = v;}
    float capacity(){return m_capacity;}
    void capacity(float v){m_capacity = v;}

    void calculateState();
    void setSOHTracking(bool state);
    void simData();
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
    int m_ovCounts = 0;
    int m_ocCounts = 0;
    int m_voltHighThreshold = 1000;
    int m_voltLowThreshold = 1000;
    int m_currentHighThreshold = 100;
    int m_currentLowThreshold = 90;
    int m_holdSec = 5;
    bool m_voltAlarm = false;
    bool m_currentAlarm = false;
    long long m_lastSeen,m_currentTime;
    float m_soc = 100, m_soh = 100;
    int m_simVolt = 0, m_simAmpere = 0;
    int m_simSOC = 0;
    float m_capacity = 50; // Battery's capacity,AH
    bool m_sohTrack = false;
    double m_sohAccum = 0;
};

#endif // BMS_SVIDEVICE_H
