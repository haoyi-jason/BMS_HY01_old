#ifndef BMS_STACK_H
#define BMS_STACK_H

#include <QObject>
#include <QtCore>

class BMS_BMUDevice;
class BMS_BCUDevice;
class BMS_SVIDevice;

const QHash<QString,int> HEADER = {
    {"SOC",0},{"STV",1},{"STC",2},
    {"CVH",3},{"CVHID",4},{"CVL",5},
    {"CVLID",6},{"STH",7},{"STHID",8},
    {"STL",9},{"STLID",10},{"DIFF",11},
};

class BMS_Stack : public QObject
{
    Q_OBJECT
public:
    explicit BMS_Stack(QObject *parent = nullptr);
    int BatteryCount();
    ushort cellVoltage(int bid, int cid);
    void cellVoltage(int bid, int cid, ushort x);
    ushort packTemp(int bid, int tid);
    void packTemp(int bid, int tid, ushort x);
    int32_t queueData(int bid, int cid);
    void queueData(int bid, int cid, ushort x);
    void addBattery(BMS_BMUDevice *battery);
    void setHVC(BMS_SVIDevice *hvc);
    ushort soc();
    void set_soc(ushort soc);
    ushort soh();
    void soh(ushort x);
    ushort maxCellVoltage();
    void maxCellVoltage(ushort x);
    int maxCellVoltIndex();
    void maxCellVoltIndex(int x);

    ushort minCellVoltage();
    void minCellVoltage(ushort x);
    int minCellVoltIndex();
    void minCellVoltIndex(int x);

    ushort cellVoltDiff();
    void cellVoltDiff(ushort x);

    ushort maxStackTemperature();
    void maxStackTemperature(ushort x);
    ushort minStackTemperature();
    void minStackTemperature(ushort x);
    quint32 stackVoltage();
    void stackVoltage(quint32 x);
    short stackCurrent();
    void stackCurrent(short x);
    QString alias();
    void alias(QString x);
    int groupID();
    void groupID(int value);
    void enableHVModule();
    void feedData(quint32 identifier, QByteArray data);
    void simData();
    void setSimCellData(quint8 id, quint8 cell, ushort v );
    void setSimTempData(quint8 id, quint8 cell, ushort v );
//    void setSimStackVoltage(quint8 gid, int v);
//    void setSimStackAmpere(quint8 gid, int v);
    QByteArray data();
    friend QDataStream &operator<<(QDataStream &out, const BMS_Stack *stack);
    friend QDataStream &operator >> (QDataStream &in, BMS_Stack *stack);
    QList<BMS_BMUDevice *> batteries();
    static QStringList headerInfo()
    {
        QHash<QString, int> m_params;
        m_params.insert("SOC",0);
        m_params.insert("Stack Volt.",1);
        m_params.insert("Stack Current",2);
        m_params.insert("CVH",3);
        m_params.insert("CVH_ID",4);
        m_params.insert("CVL",5);
        m_params.insert("CVL_ID",6);
        m_params.insert("STH",7);
        m_params.insert("STH_ID",8);
        m_params.insert("STL",9);
        m_params.insert("STL_ID",10);
        m_params.insert("Diff",11);
       QStringList lst;
       for(int i=0;i<HEADER.size();i++){
           lst << HEADER.keys(i);
       }
       return  lst;
    }

    QString state();
    void state(QString v);
    BMS_SVIDevice *sviDevice() const;
    quint32 alarmState();
    void clearAlarm();
signals:

public slots:

private:
    BMS_SVIDevice *m_svi=nullptr;  // stack voltage/current
    QList<BMS_BMUDevice*> m_batteries; // point to BMS_BMUDevice
    ushort m_MaxCellVoltage;
    int m_MaxCellIndex;
    ushort m_MinCellVoltage;
    int m_MinCellIndex;
    ushort m_MaxTemperature;
    ushort m_MinTemperature;
    quint32 m_StackVoltage;
    short m_StackCurrent;
    QString m_State;
    ushort m_soc;
    ushort m_soh;
    QString m_alias;
    uint8_t m_groupID;
    ushort m_CellVoltDiff;
    ushort m_simStackVShift=0, m_simStackAShift = 0;
};


class BMS_StackConfig:public QObject
{
    Q_OBJECT
public:
    BMS_StackConfig(){
        m_nofBatteries = 20;
        m_nofCellPerBattery  =12;
        m_nofNTCPerBattery = 5;
        m_groupID = 1;
    }

//private:
    QString m_name;
    int m_nofBatteries;
    int m_nofCellPerBattery;
    int m_nofNTCPerBattery;
    int m_groupID;
    bool m_hvboard;

    friend QDataStream& operator << (QDataStream &out, BMS_StackConfig *cfg){
        out << cfg->m_nofBatteries;
        out << cfg->m_nofCellPerBattery;
        out << cfg->m_nofNTCPerBattery;
        out << cfg->m_groupID;
        out << cfg->m_hvboard;
        return out;
    }

    friend QDataStream& operator >> (QDataStream &in, BMS_StackConfig *cfg){
        in >> cfg->m_nofBatteries;
        in >> cfg->m_nofCellPerBattery;
        in >> cfg->m_nofNTCPerBattery;
        in >> cfg->m_groupID;
        in >> cfg->m_hvboard;
        return in;
    }
};

#endif // BMS_STACK_H
