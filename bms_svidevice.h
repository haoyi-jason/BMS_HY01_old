#ifndef BMS_SVIDEVICE_H
#define BMS_SVIDEVICE_H

#include <QObject>

class BMS_HVCInfo : public QObject
{
    Q_OBJECT
public:
    explicit BMS_HVCInfo(QObject *parent = nullptr);
    QByteArray data();
    int voltage();
    int current();
    void feedData(quint8 id, quint16 msg, QByteArray data);
    friend QDataStream& operator << (QDataStream &out, const BMS_HVCInfo *hvc);
    friend QDataStream& operator >> (QDataStream &in, BMS_HVCInfo *hvc);

signals:

public slots:

private:
    int m_stackVoltage;
    int m_stackCurrent;
    QList<ushort> m_auxInputs;

};

#endif // BMS_SVIDEVICE_H
