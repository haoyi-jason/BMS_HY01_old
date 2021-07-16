#ifndef BMS_SYSTEM_H
#define BMS_SYSTEM_H

#include <QObject>
#include "bms_def.h"
#include "bms_localconfig.h"

class BMS_BMUDevice;
class BMS_BCUDevice;
class BMS_SVIDevice;
class BMS_Stack;
class BMS_StackConfig;
class QTimer;


class BMS_System : public QObject
{
    Q_OBJECT
public:

    explicit BMS_System(QObject *parent = nullptr);
    ~BMS_System();
    void SetStackInfo(QList<BMS_Stack*> info);
    void AddStack(BMS_Stack *stack);
    void ClearStack();
    BMS_Stack* findStack(QString stackName);
    bool Configuration(QByteArray data);
    QByteArray Configuration();
    bool Configuration2(QString path);
    bool Configuration2(QByteArray b);
    QString alias(){return m_alias;}
    void alias(QString value){m_alias = value;}
    QList<BMS_Stack*> stacks(){return m_stacks;}
    void generateSystemStructure();
    //void generateDummySystem();
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

    friend QDataStream &operator<<(QDataStream &out, const BMS_System *sys);
    friend QDataStream &operator>>(QDataStream &in, BMS_System *sys);
    //CAN_Packet* setDigitalOut(int ch, int value);
    //CAN_Packet* setVoltageSource(int ch, int value, bool enable);
    void flushBCU();
    QByteArray digitalInput();
    QByteArray digitalOutput();
    QList<int> vsource();
    void rec_log(QByteArray data);
    void rec_log_csv();
    void emg_log(QByteArray data);
    void emg_log(QString msg);
    void logPath(QString path);
    QString logPath(){return m_logPath;}
    BMS_BCUDevice* bcu();
    QList<int> batteriesPerStack();
    CAN_Packet *setBalancing(quint16 bv, quint8 bh, quint8 be, quint16 on, quint16 off);
    CAN_Packet *broadcastBalancing();
    CAN_Packet *startBMUs(bool enable);
    CAN_Packet *heartBeat();
    bool enableLog();
    void enableLog(bool enable);
    int logDays();
    void logDays(int days);
    int logRecords();
    void logRecords(int recs);

    BMS_LocalConfig *config();

    void enableAlarmSystem(bool en);
    quint32 alarmState();
    void clearAlarm();
    int warinig_out_id(){return m_warning_out_id;}
    bool warinig_latch(){return m_warning_latch;}
    int alarm_out_id(){return m_alarm_out_id;}
    bool alarm_latch(){return m_alarm_latch;}
    QDateTime startTime(){return m_startTime;}
    void startTime(qint64 epoch){m_startTime = QDateTime::fromSecsSinceEpoch(epoch);}
    ushort miniCellVoltage(){return m_cellMinVoltage;}
    ushort minSID(){return m_minVSID;}
    ushort minBID(){return m_minVBID;}
    ushort minCID(){return m_minVCID;}

    bool isSimulate(){return m_useSimulator;}
    BMS_LocalConfig *localConfig(){return m_localConfig;}

    void ms_poll_100();
signals:
    void sendPacket(QByteArray data);
    void setBalancingVoltage(ushort v);
    void deviceLost(quint8 devid);
    void logMessage(QString);
private slots:
    void simulate();
    void validState();
    void saveCurrentSOC();
    void checkDiskSpace();
    void eventRecord();

public slots:
    void On_BMU_ov(quint16 mask);
    void On_BMU_uv(quint16 mask);
    void On_BMU_ot(quint16 mask);
    void On_BMU_ut(quint16 mask);
    void On_SVI_ov();
    void On_SVI_oc();
    void log(QString msg);
    void sys_log(QString msg);
    void evt_log(QString evName, QString evLevel, QString State, QString evInfo);

private:
    QList<BMS_Stack*> m_stacks;
    QList<BMS_StackConfig*> m_stackConfig;
    QString m_alias;
    QTimer *m_simulateTimer=nullptr;
    BMS_BCUDevice *m_bcuDevice=nullptr;
    QString m_logPath="./";
    bool m_loadFromFile=false;
    bool m_enableLog = false;
    int m_logDays = -1;
    int m_logRecords = -1;
    QTimer *m_validTimer = nullptr;
    int m_validInterval = 5;
    bool m_simulating = false;
    int m_alarm_out_id=0;
    int m_warning_out_id = 0;
    bool m_warning_latch = true;
    bool m_alarm_latch = true;
    //QList<CANBUSDevice*> m_canbusDevice;
    //MODBUSDevice *m_modbusDev = nullptr;
    ushort m_currentBalanceVoltage;
    ushort m_cellMinVoltage;
    ushort m_cellMaxVoltage;
    QDateTime m_startTime;
    ushort m_minVSID, m_minVBID, m_minVCID;
    BMS_LocalConfig *m_localConfig;
    quint16 m_eventLogSize = 0xffff;
    quint16 m_maxEvents = 550;
    bool m_useSimulator = false;
    quint32 m_socSaveDelay = 300;
    QTimer *m_eventRecordTimer;
    quint32 m_eventRecordCounter=0;
    quint16 m_eventRecordSize = 0xffff;
    quint16 m_maxEventRecords;
};

#endif // BMS_SYSTEM_H
