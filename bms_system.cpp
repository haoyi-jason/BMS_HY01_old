#include "bms_system.h"
#include "bms_def.h"
#include "bms_bmudevice.h"
#include "bms_bcudevice.h"
#include "bms_svidevice.h"
#include "bms_stack.h"


BMS_System::BMS_System(QObject *parent) : QObject(parent)
{

}
void BMS_System::SetStackInfo(QList<BMS_StackInfo*> info)
{
    m_stacks = info;
}

void BMS_System::AddStack(BMS_StackInfo *stack)
{
    m_stacks.append(stack);
}

void BMS_System::ClearStack()
{
    m_stacks.clear();
}

BMS_StackInfo* BMS_System::findStack(QString stackName)
{
    BMS_StackInfo *stack = nullptr;
    foreach(BMS_StackInfo *s, m_stacks){
        if(s->alias() == stackName){
            stack = s;
        }
    }
    return stack;
}

bool BMS_System::Configuration(QByteArray data)
{
    bool ret = false;
    QJsonParseError e;
    QJsonDocument d = QJsonDocument::fromJson(data,&e);
    if(d.isNull()){
        return false;
    }
    QJsonObject obj = d.object();

    this->Alias = (obj["alias"].toString());
    this->connectionString = obj["host_ip"].toString();
    this->connectionPort = obj["port"].toInt();

    if(obj.contains("balancing")){
        QJsonObject bal = obj["balancing"].toObject();
        this->BalancingVoltage = bal["voltage"].toInt();
        this->BalancingHystersis = bal["hystersis"].toInt();
        this->BalancingOnTime = bal["on_time"].toInt();
        this->BalancingOffTime = bal["off_time"].toInt();
    }

    if(obj.contains("report")){
        QJsonObject rpt = obj["report"].toObject();
        this->normalReportMS = rpt["normal"].toInt();
        this->errorReportMS = rpt["error"].toInt();
    }

    if(obj.contains("stacks")){
        m_stackConfig.clear();
        QJsonArray ja = obj["stacks"].toArray();
        Stacks = 0;
        foreach (QJsonValue o, ja) {
            BMS_StackConfig *cfg = new BMS_StackConfig();
            cfg->m_nofBatteries = o["baterries_per_stack"].toInt();
            cfg->m_nofCellPerBattery = o["cells_per_battery"].toInt();
            cfg->m_nofNTCPerBattery = o["ntcs_per_stack"].toInt();
            cfg->m_hvboard = o["hv_module"].toBool();
            cfg->m_groupID = o["group_id"].toInt();

            m_stackConfig.append(cfg);
            Stacks++;
        }
        ret = true;
    }
    else if(obj.contains("stack")){
        QJsonObject ja = obj["stack"].toObject();
        this->Stacks = ja["count"].toInt();
        for(int i=0;i<Stacks;i++){
            BMS_StackConfig *cfg = new BMS_StackConfig();
            cfg->m_nofBatteries = ja["baterries_per_stack"].toInt();
            cfg->m_nofCellPerBattery = ja["cells_per_battery"].toInt();
            cfg->m_nofNTCPerBattery = ja["ntcs_per_stack"].toInt();
            cfg->m_hvboard = ja["hv_module"].toBool();
            cfg->m_groupID = i+1;
            m_stackConfig.append(cfg);
        }
        ret = true;
    }
    if(obj.contains("bcu")){
        this->m_bcuDevice = new BMS_BCUDevice();
        QJsonObject bcu = obj["bcu"].toObject();
        if(bcu.contains("digital")){
            QJsonObject digital = bcu["digital"].toObject();
            if(digital.contains("input")){
//                QJsonObject di = digital["input"].toObject();
                int n = digital["input"].toObject()["channels"].toInt();
                m_bcuDevice->add_digital_input(n);
            }
            if(digital.contains("output")){
                int n = digital["output"].toObject()["channels"].toInt();
                m_bcuDevice->add_digital_output(n);
            }

        }
        if(bcu.contains("analog")){
            QJsonObject analog = bcu["anlog"].toObject();
            if(analog.contains("input")){
                int n = analog["input"].toObject()["channels"].toInt();
                m_bcuDevice->add_analog_input(n);
            }
            //if(analog.contains("pwm_in")){
                int n = analog["pwm_in"].toObject()["channels"].toInt();
                m_bcuDevice->add_pwm_in(2);
            //}
        }
        if(bcu.contains("voltage_source")){
            QJsonObject vs = bcu["voltage_source"].toObject();
            if(vs.contains("channels")){
                int n = vs["channels"].toInt();
                m_bcuDevice->add_voltage_source(n);
            }
        }
    }
    if(ret){
        generateSystemStructure();
    }
    return ret;
}

QByteArray BMS_System::Configuration()
{
    QByteArray ret;

    return ret;
}


void BMS_System::generateSystemStructure()
{
    m_stacks.clear();
    foreach(BMS_StackConfig *c, m_stackConfig)
    {
        BMS_StackInfo *info = new BMS_StackInfo();
        quint8 id = 1; // bmu id start from 1
        for(int i=0;i<c->m_nofBatteries;i++){
            BMS_BMUDevice *bat = new BMS_BMUDevice(c->m_nofCellPerBattery,c->m_nofNTCPerBattery);
            bat->deviceID(id++);
            info->addBattery(bat);
        }
        info->enableHVModule();
//        if(c->m_hvboard){
//            info->enableHVModule();
//        }
        info->groupID(c->m_groupID);

        m_stacks.append(info);
    }

}

void BMS_System::generateDummySystem()
{
    qDebug()<<"generate dummy system";
    m_stacks.clear();
    for(int i=1;i<8;i++) // 7 stacks
    {
        BMS_StackInfo *info = new BMS_StackInfo();
        for(int j=0;j<20;j++){
            BMS_BMUDevice *bat = new BMS_BMUDevice(12,5);
            bat->deviceID(GROUP(i)+j+2);
            info->addBattery(bat);
        }
        info->enableHVModule();
        info->groupID((i));

        m_stacks.append(info);
    }
}

void BMS_System::feedData(quint32 identifier, QByteArray data)
{
    uint8_t id = (identifier >> 12) & 0xff;
    quint16 cmd = identifier & 0xFFF;
    if(id == 0x01){ // bcu device
        this->m_bcuDevice->feedData(id,cmd,data);
    }
    else{
        foreach(BMS_StackInfo *s,m_stacks){
            s->feedData(identifier,data);
        }
    }
}

void BMS_System::startSimulator(int interval)
{
    m_simulateTimer = new QTimer();
    connect(m_simulateTimer,&QTimer::timeout,this,&BMS_System::simulate);
    m_simulateTimer->start(interval);
}

void BMS_System::stopSimulator()
{
    m_simulateTimer->stop();
    m_simulateTimer->deleteLater();
}

void BMS_System::simulate()
{
    //feed data to stacks
    ushort sim_data[4];
    QByteArray data;
    QDataStream ds(&data, QIODevice::ReadWrite);

    quint32 total_voltage = 0;
    foreach(BMS_StackInfo *s,m_stacks){
        s->dummyData();
    }


}

QByteArray BMS_System::data()
{
    // loop through each stack to gather data
    QByteArray d;
    QDataStream s(&d,QIODevice::WriteOnly);
    foreach(BMS_StackInfo *b,m_stacks){
        s << b->data();
    }

    return d;
}


QDataStream& operator<<(QDataStream &out, const BMS_System *sys)
{
    out << sys->Stacks;
    out << sys->m_bcuDevice;
    foreach (BMS_StackInfo *s, sys->m_stacks) {
        out << s;
    }
    return out;
}

QDataStream& operator >> (QDataStream &in, BMS_System *sys)
{
    in >> sys->Stacks;
//        if(sys->m_loadFromFile){
//            for(int i=0;i<sys->Stacks; i++){
//                BMS_StackConfig *s = new BMS_StackConfig();
//                sys->m_stackConfig.append(s);
//            }
//        }
//        foreach(BMS_StackConfig *s,sys->m_stackConfig){
//            in >> s;
//        }

//        if(sys->m_loadFromFile){
//            sys->generateSystemStructure();
//            sys->m_bcuDevice = new BMS_BCUDevice();
//        }
    if(sys->m_bcuDevice == nullptr){
        sys->m_bcuDevice = new BMS_BCUDevice();
    }
    in >> sys->m_bcuDevice;
    if(sys->m_stacks.size() == 0){
        for(quint8 i=0;i<sys->Stacks;i++){
            sys->m_stacks.append(new BMS_StackInfo());
        }
    }
    foreach (BMS_StackInfo *s, sys->m_stacks) {
        in >> s;
    }
    return in;
}
CAN_Packet* BMS_System::setDigitalOut(int ch, int value){
    if(m_bcuDevice != nullptr){
        return m_bcuDevice->setDigitalOut(ch,value);
    }
    return nullptr;
}
CAN_Packet* BMS_System::setVoltageSource(int ch, int value, bool enable){
    if(m_bcuDevice != nullptr){
        return m_bcuDevice->setVoltageSource(ch,value,enable);
    }
}

void BMS_System::flushBCU(){
    if(m_bcuDevice != nullptr){
        m_bcuDevice->generatePacket();
    }
}

QByteArray BMS_System::digitalInput(){
    if(m_bcuDevice != nullptr){
        return m_bcuDevice->getDigitalInput();
    }
}

QByteArray BMS_System::digitalOutput(){
    if(m_bcuDevice != nullptr){
        return m_bcuDevice->getDigitalOutput();
    }
}

QList<int> BMS_System::vsource(){
    if(m_bcuDevice != nullptr){
        return m_bcuDevice->getWorkingCurrent();
    }
}

void BMS_System::log(QString path, QByteArray data){
    //QString path = QString("%1/%2.bin").arg(m_logPath).arg(QDateTime::currentMSecsSinceEpoch(),16,16,QLatin1Char('0'));
    QFile f(path);
    if(f.open(QIODevice::WriteOnly)){
        QDataStream ds(&f);
        ds << data;
        f.close();
    }
}

void BMS_System::emg_log(QByteArray data){

}

void BMS_System::logPath(QString path){m_logPath = path;}

BMS_BCUDevice* BMS_System::bcu(){return m_bcuDevice;}

QList<int> BMS_System::batteriesPerStack()
{
    QList<int> ret;
    foreach(BMS_StackInfo *s,m_stacks){
        ret << s->BatteryCount();
    }
    if(ret.size() == 0){
        ret << 0;
    }
    return ret;
}
