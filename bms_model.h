#ifndef BMS_MODEL_H
#define BMS_MODEL_H
#include <QObject>
#include <QtCore>

namespace bms {
#define GROUP(x)    (x >> 5)
#define ID(x)       (x & 0x1F)
    const int MAX_CELLS = 12;
    const int MAX_NTC = 5;
}

class BMS_BatteryInfo:public QObject{
    Q_OBJECT
public:
    BMS_BatteryInfo(QObject *parent = nullptr);
    BMS_BatteryInfo(int nofCells, int nofTemp,QObject *parent = nullptr);
    ushort getCellVoltage(int index);
    ushort getPackTemperature(int index);

    ushort cellVoltage(int index){return (index<m_cellVoltage.size())?m_cellVoltage[index]:0;}
    QList<ushort> cellVoltages(){return m_cellVoltage;}
    void cellVoltage(int id, ushort x){m_cellVoltage[id] = x;}
    void cellVoltages(QList<ushort>x){m_cellVoltage = x;}

    ushort packTemperature(int index){return (index<m_packTemperature.size())?m_packTemperature[index]:0;}
    QList<ushort> packTemperatures(){return m_packTemperature;}
    void packTemperature(int id, ushort x){m_packTemperature[id] = x;}
    void packTemperatures(QList<ushort>x){m_packTemperature = x;}

    ushort balancing(int index){return (index<m_balancing.size())?m_balancing[index]:0;}
    QList<ushort> balancing(){return m_balancing;}
    void balancing(QList<ushort>x){m_balancing = x;}
    void balancing(int index, ushort x){(index < m_balancing.size())?m_balancing[index]=x:0;}
    void balancingVoltage(ushort value){m_balancingVoltage = value;}
    void balancingHystersis(ushort value){m_balancingHystersis = value;}
    void balancingONTime(ushort value){m_balancingONSeconds = value;}
    void balancingOFFTime(ushort value){m_balancingOFFSeconds = value;}
    void feedData(uint8 id, uint16_t msg, QByteArray data){
        if(ID(id) == m_devid){
            switch (msg) {
            case value:

                break;
            default:
                break;
            }
        }
    }
private:
    QList<ushort> m_cellVoltage;
    QList<ushort> m_packTemperature;
    QList<ushort> m_balancing;
    ushort m_balancingVoltage;
    ushort m_balancingHystersis;
    ushort m_balancingONSeconds;
    ushort m_balancingOFFSeconds;
    uint8_t m_devid;
};

class BMS_HVCInfo:public QObject{
    Q_OBJECT
public:
    BMS_HVCInfo(){}

private:
    int m_stackVoltage;
    int m_stackCurrent;
    QList<ushort> m_auxInputs;
};

/*
 *  stack information of a BMS
 *  each stack contains n battery and at most 1 stack voltage/current module,
 *  32-devices per stack with 8-bit CANBUS device address mapping, MSB 3-bits
 *  for group, LSB 5-bit for address
 *  1. stack voltage/current module config at fixed address 0x01
 *  2. battery (at most 12-cells) start from 2 to 31, 29 batteries most
 *  3. address 0x0 (group 0/ id 0) is for broadcasting.
 */

const QHash<QString,int> HEADER = {
    {"SOC",0},{"STV",1},{"STC",2},
    {"CVH",3},{"CVHID",4},{"CVL",5},
    {"CVLID",6},{"STH",7},{"STHID",8},
    {"STL",9},{"STLID",10},{"DIFF",11},
};

class BMS_StackInfo:public QObject{
    Q_OBJECT
public:
    BMS_StackInfo();
    int BatteryCount();
    ushort cellVoltage(int bid, int cid);
    void cellVoltage(int bid, int cid, ushort x);
    ushort packTemp(int bid, int tid);
    void packTemp(int bid, int tid, ushort x);
    ushort queueData(int bid, int cid);
    void queueData(int bid, int cid, ushort x);

    void addBattery(BMS_BatteryInfo *battery);
    void setHVC(BMS_HVCInfo *hvc);
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
    ushort soc(){return m_soc;}
    void set_soc(ushort soc){m_soc = soc;}
    ushort soh(){return m_soh;}
    void soh(ushort x){m_soh = x;}
    ushort maxCellVoltage(){return m_MaxCellVoltage;}
    void maxCellVoltage(ushort x){m_MaxCellVoltage = x;}
    int maxCellVoltIndex(){return m_MaxCellIndex;}
    void maxCellVoltIndex(int x){m_MaxCellIndex = x;}
    ushort maxStackTemperature(){return m_MaxTemperature;}
    void maxStackTemperature(ushort x){m_MaxTemperature = x;}
    ushort minStackTemperature(){return m_MinTemperature;}
    void minStackTemperature(ushort x){m_MinTemperature = x;}
    ushort stackVoltage(){return m_StackVoltage;}
    void stackVoltage(ushort x){m_StackVoltage = x;}
    short stackCurrent(){return m_StackCurrent;}
    void stackCurrent(short x){m_StackCurrent = x;}
    QString alias(){return m_alias;}
    void alias(QString x){m_alias = x;}
    int groupID(){return m_groupID;}
    void groupID(int value){m_groupID = value;}
    void feedData(int32_t msgid, QByteArray data){
        uint8_t id = (msgid >> 12) & 0xff;
        uint16_t cmd = (msgid & 0xFFF);
        if(GROUP(id) == m_groupID){

        }
    }
private:
    BMS_HVCInfo *m_hvcInfo;  // stack voltage/current
    QList<BMS_BatteryInfo*> m_batteries; // point to BMS_BatteryInfo
    ushort m_MaxCellVoltage;
    int m_MaxCellIndex;
    ushort m_MinCellVoltage;
    int m_MinCellIndex;
    ushort m_MaxTemperature;
    ushort m_MinTemperature;
    ushort m_StackVoltage;
    short m_StackCurrent;
    QString m_State;
    ushort m_soc;
    ushort m_soh;
    QString m_alias;
    uint8_t m_groupID;
};

/*
 * class BMS_SystemInof
 * Object to hold system information
 *
 */
class BMS_SystemInfo:public QObject
{
    Q_OBJECT
public:
    BMS_SystemInfo();
    void SetStackInfo(QList<BMS_StackInfo*> info);
    void AddStack(BMS_StackInfo *stack);
    void ClearStack();
    BMS_StackInfo* findStack(QString stackName);
    bool Configuration(QByteArray data);
    QByteArray Configuration();
    void feedData(int msgid, QByteArray data);

private:
    QString _connection;
    int m_port;
    QList<BMS_StackInfo*> m_stacks;
    int m_normalReportMS, m_errorReportMS;
};

class BMS_BatteryModel:public QAbstractTableModel
{
    Q_OBJECT

public:
    BMS_BatteryModel(QObject *parent = nullptr);
    ~BMS_BatteryModel();
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section,Qt::Orientation orientation, int role) const override;
//    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
//    bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;
//    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;

    void feedData(); // feed data for battery info

    void setStack(BMS_StackInfo *stack);
public slots:


signals:
    void editCompleted(const QString&);

private:
    //QList<BMS_StackInfo*> m_stacks;
    BMS_StackInfo *m_activeStack;
    QStringList m_header;
};

class BMS_StackModel:public QAbstractTableModel
{
    Q_OBJECT
public:
    BMS_StackModel(QObject *parent = nullptr);
    ~BMS_StackModel();
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section,Qt::Orientation orientation, int role) const override;
    //bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    //bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;
    //bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;

    void addStack(BMS_StackInfo *stack);
    void setCurrentStack(int index);
    BMS_StackInfo *findStack(int id);
public slots:

private:
    QList<BMS_StackInfo*> m_stacks;
    QStringList m_header;
    BMS_StackInfo *m_currentStack;
};

#endif // BMS_MODEL_H
