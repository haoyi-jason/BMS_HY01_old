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
    int current();
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
signals:
    void set_ov();
    void set_uv();
    void set_oc();
    void set_uc();
public slots:
    void validAlarm();
private:
    int m_aux1,m_aux2;
    int m_stackVoltage;
    int m_stackCurrent;
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
    long long m_lastSeen;
    int m_soc;
};

#endif // BMS_SVIDEVICE_H
