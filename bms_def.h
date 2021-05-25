#ifndef BMS_DEF_H
#define BMS_DEF_H

#include <QObject>
#include <QtCore>

class QTcpSocket;
class CANBUSDevice;
class MODBUSDevice;

namespace bms {
#define GROUP_OF(x)    (x >> 5)
#define ID_OF(x)       (x & 0x1F)
#define GROUP(x)        (x<< 5)
#define ID(x)       (x)
    const int MAX_CELLS = 12;
    const int MAX_NTC = 5;
}

class HW_IOChannel{
public:
    explicit HW_IOChannel(int initValue = 0, bool inverted = false){
        m_value = initValue;
        m_valueToWrite = initValue;
        m_valueReadBack = initValue;
    }

    int value(){return m_value;}
    void writeValue(int value){m_valueToWrite = value;}
    void setReadback(int value){m_valueReadBack = value;}
    bool valid(){return (m_valueReadBack == m_value);}

private:
    int m_valueToWrite;
    int m_valueReadBack;
    int m_value;
};

class BMS_BCUDevice:public QObject
{
    Q_OBJECT
public:
    enum PendingAction{
        NO_PENDING,
        AO_PENDING,
        DO_PENDING
    };
    Q_ENUM(PendingAction)

    explicit BMS_BCUDevice(QObject *parent = nullptr){};
    void add_digital_input(int n, bool inverted=false){
        for(int i=0;i<n;i++){
            HW_IOChannel *c = new HW_IOChannel();
            m_digitalInput.append(c);
        }
    }
    void add_digital_output(int n, bool inverted=false){
        for(int i=0;i<n;i++){
            HW_IOChannel *c = new HW_IOChannel();
            m_digitalOutput.append(c);
        }
    }
    void add_analog_input(int n){
        for(int i=0;i<n;i++){
            HW_IOChannel *c = new HW_IOChannel();
            m_analogInput.append(c);
        }
    }

    void add_voltage_source(int n){
        for(int i=0;i<n;i++){
            HW_IOChannel *c = new HW_IOChannel();
            m_voltageSource.append(c);
        }
    }

    void feedData(quint8 id, quint16 msg, QByteArray data){
        if(id == 0x01){ // BCU always occupied 0x01, group 0, id 1
            QDataStream ds(&data,QIODevice::ReadOnly);
            quint16 uv;
            quint8 bv;
            switch(msg){
                case 0x121:
                    ds >> uv;
                    m_pwmInput[0]->setReadback(uv);
                    ds >> uv;
                    m_pwmInput[1]->setReadback(uv);
                    ds >> uv;
                    m_voltageSource[0]->setReadback(uv);
                    ds >> uv;
                    m_voltageSource[1]->setReadback(uv);
                    break;
                case 0x122:
                    ds >> bv;
                    for(int i=0;i<2;i++){
                        if((bv & (1 << i)) == (1 << i)){
                            m_digitalInput[i]->setReadback(1);
                        }
                        else{
                            m_digitalInput[i]->setReadback(0);
                        }
                    }
                    ds >> bv;
                    for(int i=0;i<2;i++){
                        if((bv & (1 << i)) == (1 << i)){
                            m_digitalOutput[i]->setReadback(1);
                        }
                        else{
                            m_digitalOutput[i]->setReadback(0);
                        }
                    }
                break;
                default:break;
            }
        }
    }

    int isActionPending(){
        int ret = NO_PENDING;
        // check do first
        foreach(HW_IOChannel *c,m_digitalOutput){
            if(!c->valid()){
                ret = DO_PENDING;
            }
        }
        foreach (HW_IOChannel *c, m_analogInput) {
            if(!c->valid()){
                ret |= AO_PENDING;
            }
        }
        return ret;
    }

    QByteArray genPacket(PendingAction action){
        QByteArray ret;
        switch(action){
        case DO_PENDING:
            ret.append(m_digitalOutput[0]->value() | (1<< m_digitalOutput[1]->value()));
            break;
        case AO_PENDING:
            break;
        default:break;
        }

        return ret;
    }

    friend QDataStream& operator << (QDataStream &out, const BMS_BCUDevice *dev){
        quint8 bv = 0;
        if(dev->m_digitalInput.size() > 0){
            bv = (0 << dev->m_digitalInput[0]->value()) | (1 << dev->m_digitalInput[1]->value());
            out << bv;
        }
        if(dev->m_digitalOutput.size() > 0){
            bv = (0 << dev->m_digitalOutput[0]->value()) | (1 << dev->m_digitalOutput[1]->value());
            out << bv;
        }

        if(dev->m_analogInput.size() > 0){
            foreach(HW_IOChannel *c, dev->m_analogInput){
                out << c->value();
            }
        }
        if(dev->m_pwmInput.size() > 0){
            foreach(HW_IOChannel *c, dev->m_pwmInput){
                out << c->value();
            }
        }
        if(dev->m_voltageSource.size() > 0){
            foreach(HW_IOChannel *c, dev->m_voltageSource){
                out << c->value();
            }
        }
        return out;
    }

    friend QDataStream& operator >> (QDataStream &in, BMS_BCUDevice *dev){
        quint8 bv;
        int v;
        if(dev->m_digitalInput.size() > 0){
            in >> bv;
            dev->m_digitalInput[0]->setReadback((bv&0x1)?1:0);
            dev->m_digitalInput[1]->setReadback((bv&0x2)?1:0);
        }
        if(dev->m_digitalOutput.size() > 0){
            in >> bv;
            dev->m_digitalOutput[0]->setReadback((bv&0x1)?1:0);
            dev->m_digitalOutput[1]->setReadback((bv&0x2)?1:0);
        }

        if(dev->m_analogInput.size() > 0){
            foreach (HW_IOChannel *c, dev->m_analogInput) {
                in >> v;
                c->setReadback(v);
            }
        }

        if(dev->m_pwmInput.size() > 0){
            foreach (HW_IOChannel *c, dev->m_pwmInput) {
                in >> v;
                c->setReadback(v);
            }
        }

        if(dev->m_voltageSource.size() > 0){
            foreach (HW_IOChannel *c, dev->m_voltageSource) {
                in >> v;
                c->setReadback(v);
            }
        }
        return in;
    }

private:
    QList<HW_IOChannel*> m_digitalInput;
    QList<HW_IOChannel*> m_digitalOutput;
    QList<HW_IOChannel*> m_analogInput;
    QList<HW_IOChannel*> m_pwmInput;
    QList<HW_IOChannel*> m_voltageSource;
};



class BMS_BMUDevice:public QObject{
    Q_OBJECT
public:
    BMS_BMUDevice(QObject *parent = nullptr);
    BMS_BMUDevice(int nofCells, int nofTemp,QObject *parent = nullptr);
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
    quint8 deviceID(){return m_devid;}

    void deviceID(quint8 value){m_devid = value;}
    quint8 cellCount(){return m_nofCell;}
    quint8 ntcCount(){return m_nofNtc;}
    ushort minCellVoltage(){return m_minVoltage;}
    ushort maxCellVoltage(){return m_maxVoltage;}
    ushort totalVoltage(){return m_totalVoltage;}
    ushort minPackTemp(){return m_minTemperature;}
    ushort maxPackTemp(){return m_maxTemperature;}
    ushort cellVoltageDiff(){return (m_maxVoltage>m_minVoltage)?(m_maxVoltage-m_minVoltage):0;}

    void feedData(uint8_t id, uint16_t msg, QByteArray data){
        if(id == m_devid){
            QDataStream ds(&data,QIODevice::ReadOnly);
            switch (msg) {
            case 0x110:
                for(int i=0;i<4;i++){
                    ds >> m_cellVoltage[i];
                }
                break;
            case 0x111:
                for(int i=0;i<4;i++){
                    ds >> m_cellVoltage[i+4];
                }
                break;
            case 0x112:
                for(int i=0;i<4;i++){
                    ds >> m_cellVoltage[i+8];
                }
                break;
            case 0x113:
                for(int i=0;i<4;i++){
                    ds >> m_packTemperature[i];
                }
                break;
            case 0x114:
                ds >> m_packTemperature[4];
                break;
            case 0x115:{
                ushort balancing,openWire;
                ds >> balancing;
                ds >> openWire;
                for(int i=0;i<m_cellVoltage.count();i++){
                    if((balancing & (1 <<i)) == (1 << i)){
                        m_balancing[i] = 1;
                    }
                    else{
                        m_balancing[i] = 0;
                    }
                    if((openWire & (1 <<i)) == (1 << i)){
                        m_openWire[i] = 1;
                    }
                    else{
                        m_openWire[i] = 0;
                    }
                }
            }
                break;
            default:
                break;
            }
        }
    }

    void dummyData(ushort vbase=3400, ushort vgap=100, ushort tbase = 25, ushort tgap = 5){
        for(int i=0;i<m_cellVoltage.size();i++){
            m_cellVoltage[i] = (ushort)(vbase + QRandomGenerator::global()->bounded(vgap));
        }
        for(int i=0;i<m_packTemperature.size();i++){
            m_packTemperature[i] = (ushort)(tbase + QRandomGenerator::global()->bounded(tgap));
        }
    }
    QByteArray data(){
        QByteArray d;
        QDataStream s(&d,QIODevice::WriteOnly);
        foreach(ushort u, m_cellVoltage){
            s << u;
        }
        foreach(ushort u,m_packTemperature){
            s << u;
        }
        foreach(ushort u, m_balancing){
            s << u;
        }
        return d;
    }
    friend QDataStream& operator << (QDataStream &out, const BMS_BMUDevice *bat){
        out << bat->m_devid;
        out << bat->m_nofCell;
        out << bat->m_nofNtc;
        for(int i=0;i<bat->m_cellVoltage.size();i++){
            out << bat->m_cellVoltage[i];
        }
        for(int i=0;i<bat->m_packTemperature.size();i++){
            out << bat->m_packTemperature[i];
        }
        for(int i=0;i<bat->m_balancing.size();i++){
            out << bat->m_balancing[i];
        }
//        out << bat->m_cellVoltage;
//        out << bat->m_packTemperature;
//        out << bat->m_balancing;
        return out;
    }
    friend QDataStream& operator >> (QDataStream &in, BMS_BMUDevice *bat){
        in >> bat->m_devid;
        in >> bat->m_nofCell;
        in >> bat->m_nofNtc;
        ushort max_v = 0,max_t = 0;
        ushort min_v = 0xffff,min_t = 100;
        bat->m_totalVoltage = 0;
        for(int i=0;i<bat->m_cellVoltage.size();i++){
            in >> bat->m_cellVoltage[i];
            max_v = bat->m_cellVoltage[i]>max_v?bat->m_cellVoltage[i]:max_v;
            min_v = bat->m_cellVoltage[i]<min_v?bat->m_cellVoltage[i]:min_v;
            bat->m_totalVoltage += bat->m_cellVoltage[i];
        }
        for(int i=0;i<bat->m_packTemperature.size();i++){
            in >> bat->m_packTemperature[i];
            max_t = bat->m_packTemperature[i]>max_t?bat->m_packTemperature[i]:max_t;
            min_t = bat->m_packTemperature[i]<min_t?bat->m_packTemperature[i]:min_t;
        }
        for(int i=0;i<bat->m_balancing.size();i++){
            in >> bat->m_balancing[i];
        }
        bat->m_maxVoltage = max_v;
        bat->m_maxTemperature = max_t;
        bat->m_minVoltage = min_v;
        bat->m_minTemperature = min_t;
        return in;
    }

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
};

class BMS_HVCInfo:public QObject{
    Q_OBJECT
public:
    BMS_HVCInfo(){}
    QByteArray data(){
        QByteArray d;
        QDataStream s(&d,QIODevice::WriteOnly);
        s << m_stackVoltage;
        s << m_stackCurrent;
        return d;
    }

    int voltage(){return m_stackVoltage;}
    int current(){return m_stackCurrent;}

    friend QDataStream& operator << (QDataStream &out, const BMS_HVCInfo *hvc)
    {
        out << hvc->m_stackVoltage;
        out << hvc->m_stackCurrent;
        return out;
    }

    friend QDataStream& operator >> (QDataStream &in, BMS_HVCInfo *hvc)
    {
        in >> hvc->m_stackVoltage;
        in >> hvc->m_stackCurrent;
        return in;
    }

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

    void addBattery(BMS_BMUDevice *battery);
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

    ushort minCellVoltage(){return m_MinCellVoltage;}
    void minCellVoltage(ushort x){m_MinCellVoltage = x;}
    int minCellVoltIndex(){return m_MinCellIndex;}
    void minCellVoltIndex(int x){m_MinCellIndex = x;}

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
    void enableHVModule(){m_hvcInfo = new BMS_HVCInfo();}
    void feedData(quint32 identifier, QByteArray data){
        uint8_t id = (identifier >> 12) & 0xff;
        uint16_t cmd = (identifier & 0xFFF);
        if(GROUP_OF(id) == m_groupID){
            foreach(BMS_BMUDevice *b, m_batteries){
                b->feedData(ID(id),cmd,data);
            }
        }
    }
    void dummyData(){
        foreach (BMS_BMUDevice *b, m_batteries) {
            b->dummyData();
        }
    }
    QByteArray data()
    {
        QByteArray d;
        QDataStream s(&d,QIODevice::WriteOnly);

        s << m_groupID;
        s << m_soc;
        s << m_soh;

        foreach(BMS_BMUDevice *b, m_batteries){
            //data.append(b->data());
            //s << b->data();
            s << b;
        }
        if(m_hvcInfo != nullptr){
            //data.append(m_hvcInfo->data());
            s << m_hvcInfo->data();
        }
        return d;
    }
    friend QDataStream& operator << (QDataStream &out, const BMS_StackInfo *stack){
        out << stack->m_groupID;
        //out << stack->m_MaxCellVoltage;
        //out << stack->m_MaxCellIndex;

        out << stack->m_hvcInfo;
        foreach (BMS_BMUDevice *b, stack->m_batteries) {
            out << b;
        }
        return out;
    }

    friend QDataStream& operator >> (QDataStream &in, BMS_StackInfo *stack){
        in >> stack->m_groupID;
        in >> stack->m_hvcInfo;
        ushort max_v = 0, max_t = 0;
        ushort min_v = 0xffff, min_t = 0xffff;
        int max_v_index=0, max_t_index = 0;
        int min_v_index=0, min_t_index=0;
        ushort totalVoltage = 0;
        for(int i=0;i<stack->m_batteries.size();i++){
//        foreach (BMS_BMUDevice *b, stack->m_batteries) {
            BMS_BMUDevice *b = stack->m_batteries[i];
            in >> b;
            if(b->maxCellVoltage()>max_v){
                max_v = b->maxCellVoltage();
                max_v_index = i;
            }
            if(b->minCellVoltage()<min_v){
                min_v = b->minCellVoltage();
                min_v_index = i;
            }
            if(b->maxPackTemp() > max_t){
                max_t = b->maxPackTemp();
                max_t_index = i;
            }
            if(b->minPackTemp() < min_t){
                min_t = b->minPackTemp();
                min_t_index = i;
            }
            totalVoltage += b->totalVoltage();
        }

        stack->m_StackVoltage = stack->m_hvcInfo->voltage()!=0?stack->m_hvcInfo->voltage():totalVoltage;

        stack->m_StackCurrent = stack->m_hvcInfo->current();
        stack->m_MaxCellVoltage = max_v;
        stack->m_MinCellVoltage = min_v;
        stack->m_MaxCellIndex = max_v_index;
        stack->m_MinCellIndex = min_v_index;
        stack->m_MaxTemperature = max_t;
        stack->m_MinTemperature = min_t;
        stack->m_soc = 100;
    }
private:
    BMS_HVCInfo *m_hvcInfo;  // stack voltage/current
    QList<BMS_BMUDevice*> m_batteries; // point to BMS_BMUDevice
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
};

/*
 * class BMS_SystemInfo
 * Object to hold system information
 *
 */
class BMS_SystemInfo:public QObject
{
    Q_OBJECT
public:
    BMS_SystemInfo(QObject *parent = nullptr);
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

    friend QDataStream& operator << (QDataStream &out, const BMS_SystemInfo *sys){
        out << sys->m_bcuDevice;
        foreach (BMS_StackInfo *s, sys->m_stacks) {
            out << s;
        }
        return out;
    }

    friend QDataStream& operator >> (QDataStream &in, BMS_SystemInfo *sys)
    {
        in >> sys->m_bcuDevice;
        foreach (BMS_StackInfo *s, sys->m_stacks) {
            in >> s;
        }
        return in;
    }

signals:
    void sendPacket(QByteArray data);

private slots:
    void simulate();
private:
    QList<BMS_StackInfo*> m_stacks;
    QList<BMS_StackConfig*> m_stackConfig;
    QString m_alias;
    QTimer *m_simulateTimer=nullptr;
    BMS_BCUDevice *m_bcuDevice=nullptr;
    //QList<CANBUSDevice*> m_canbusDevice;
    //MODBUSDevice *m_modbusDev = nullptr;
};

class BMS_BCUInfo:public QObject{
    Q_OBJECT
public:
    explicit BMS_BCUInfo(QObject *parent = nullptr);

private:

};

class BMS_Event:public QObject{
    Q_OBJECT
public:
    explicit BMS_Event(QObject *parent = nullptr){}

private:
    int m_evtID;
    int m_evtLevel;
    bool m_isAlarm;
    bool m_isWarning;
    QDateTime m_timeStamp;
    QString m_description;

};


class RemoteSystem{
public:
    QString connection="";
    QTcpSocket *socket = nullptr;
    BMS_SystemInfo *system = nullptr;
    bool configReady = false; // should be false
    QByteArray data;
};



#endif // BMS_DEF_H
