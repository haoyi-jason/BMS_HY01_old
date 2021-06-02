#ifndef BMS_SYSTEM_H
#define BMS_SYSTEM_H

#include <QObject>
#include "bms_def.h"

class BMS_BMUDevice;
class BMS_BCUDevice;
class BMS_HVCInfo;
class BMS_StackInfo;
class BMS_StackConfig;
class QTimer;

class BMS_System : public QObject
{
    Q_OBJECT
public:
    explicit BMS_System(QObject *parent = nullptr);
    void SetStackInfo(QList<BMS_StackInfo*> info);
    void AddStack(BMS_StackInfo *stack);
    void ClearStack();
    BMS_StackInfo* findStack(QString stackName);
    bool Configuration(QByteArray data);
    QByteArray Configuration();
    QString alias(){return m_alias;}
    void alias(QString value){m_alias = value;}
    QList<BMS_StackInfo*> stacks(){return m_stacks;}
    void generateSystemStructure();
    void generateDummySystem();
    void feedData(quint32 identifier, QByteArray data);

    void startSimulator(int interval);
    void stopSimulator();

    QByteArray data();

    QString connectionString;
    int connectionPort;
    int normalReportMS, errorReportMS;
    QString Alias;
    int Stacks;
    int BalancingVoltage;
    int BalancingHystersis;
    int BalancingOnTime;
    int BalancingOffTime;

    friend QDataStream& operator << (QDataStream &out, const BMS_System *sys);
    friend QDataStream& operator >> (QDataStream &in, BMS_System *sys);
    CAN_Packet* setDigitalOut(int ch, int value);
    CAN_Packet* setVoltageSource(int ch, int value, bool enable);
    void flushBCU();
    QByteArray digitalInput();
    QByteArray digitalOutput();
    QList<int> vsource();
    void log(QString path, QByteArray data);
    void emg_log(QByteArray data);
    void logPath(QString path);
    BMS_BCUDevice* bcu();

signals:
    void sendPacket(QByteArray data);
private slots:
    void simulate();

public slots:

private:
    QList<BMS_StackInfo*> m_stacks;
    QList<BMS_StackConfig*> m_stackConfig;
    QString m_alias;
    QTimer *m_simulateTimer=nullptr;
    BMS_BCUDevice *m_bcuDevice=nullptr;
    QString m_logPath="./";
    bool m_loadFromFile=false;
    //QList<CANBUSDevice*> m_canbusDevice;
    //MODBUSDevice *m_modbusDev = nullptr;

};

#endif // BMS_SYSTEM_H
