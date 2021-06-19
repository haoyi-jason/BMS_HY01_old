#include "bms_system.h"
#include "bms_def.h"
#include "bms_bmudevice.h"
#include "bms_bcudevice.h"
#include "bms_svidevice.h"
#include "bms_stack.h"


BMS_System::BMS_System(QObject *parent) : QObject(parent)
{
    m_startTime = QDateTime::currentDateTime();
}
void BMS_System::SetStackInfo(QList<BMS_Stack*> info)
{
    m_stacks = info;
}

void BMS_System::AddStack(BMS_Stack *stack)
{
    m_stacks.append(stack);
}

void BMS_System::ClearStack()
{
    m_stacks.clear();
}

BMS_Stack* BMS_System::findStack(QString stackName)
{
    BMS_Stack *stack = nullptr;
    foreach(BMS_Stack *s, m_stacks){
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

        qDebug()<<"Wrong configuration file";
        return false;
    }
    QJsonObject obj = d.object();

    this->Alias = obj.contains("Alias")?obj["alias"].toString():"";
    this->connectionString = obj.contains("host_ip")?obj["host_ip"].toString():"127.0.0.1";
    this->connectionPort = obj.contains("port")?obj["port"].toInt():5001;
    this->m_validInterval = obj.contains("valid_interval")?obj["valid_interval"].toInt():1;
    this->m_enableLog = obj.contains("enable_log")?obj["enable_log"].toBool():true;

    QString path;
    path = obj.contains("log_path")?obj["log_path"].toString():"log";
    if(QSysInfo::productType().contains("win")){
        this->m_logPath = "./"+path;
    }
    else{
        this->m_logPath = QCoreApplication::applicationDirPath()+ "/" + path;
    }
    this->m_logDays = obj.contains("log_days")?obj["log_days"].toInt():-1;
    this->m_logRecords= obj.contains("log_records")?obj["log_records"].toInt():200;

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

    quint16 cell_ot_set = 0, cell_ot_clr = 0;
    quint16 cell_ut_set = 0, cell_ut_clr = 0;
    quint16 cell_ov_set = 0, cell_ov_clr = 0;
    quint16 cell_uv_set = 0, cell_uv_clr = 0;
    quint16 stack_ov_set = 0, stack_ov_clr = 0;
    quint16 stack_oc_set = 0, stack_oc_clr = 0;
    quint16 duration = 5;

    if(obj.contains("criteria")){
        QJsonObject crit = obj["criteria"].toObject();
        if(crit.contains("cell")){
            QJsonObject cob = crit["cell"].toObject();
            if(cob.contains("volt-warning")){
                QJsonObject cw = cob["volt-warning"].toObject();
                if(cw.contains("high_set")) cell_ov_set = (ushort)cw["high_set"].toInt();
                if(cw.contains("high_clr")) cell_ov_clr = (ushort)cw["high_clr"].toInt();
                if(cw.contains("low_set")) cell_uv_set = (ushort)cw["low_set"].toInt();
                if(cw.contains("low_clr")) cell_uv_clr = (ushort)cw["low_clr"].toInt();
                if(cw.contains("duration")) duration = (ushort)cw["duration"].toInt();
            }
            if(cob.contains("temp-warning")){
                QJsonObject cw = cob["temp-warning"].toObject();
                if(cw.contains("high_set")) cell_ot_set = (ushort)(cw["high_set"].toDouble()*10);
                if(cw.contains("high_clr")) cell_ot_clr = (ushort)(cw["high_clr"].toDouble()*10);
                if(cw.contains("low_set")) cell_ut_set = (ushort)(cw["low_set"].toDouble()*10);
                if(cw.contains("low_clr")) cell_ut_clr = (ushort)(cw["low_clr"].toDouble()*10);
            }
        }
        if(crit.contains("stack")){
            QJsonObject sob = crit["stack"].toObject();
            if(sob.contains("volt-warning")){
                QJsonObject cw = sob["volt-warning"].toObject();
                if(cw.contains("high_set")) stack_ov_set = (ushort)cw["high_set"].toInt();
                if(cw.contains("high_clr")) stack_ov_clr = (ushort)cw["high_clr"].toInt();
            }
            if(sob.contains("current-warning")){
                QJsonObject cw = sob["current-warning"].toObject();
                if(cw.contains("high_set")) stack_oc_set = (ushort)cw["high_set"].toInt();
                if(cw.contains("high_clr")) stack_oc_clr = (ushort)cw["high_clr"].toInt();
            }
        }
    }
    if(obj.contains("event_output")){
        QJsonObject evt = obj["event_output"].toObject();
        this->m_warning_out_id = evt.contains("warning_out")?evt["warning_out"].toInt():0;
        this->m_alarm_out_id = evt.contains("alarm_out")?evt["alarm_out"].toInt():1;
        this->m_warning_latch = evt.contains("warning_latch")?evt["warning_latch"].toBool():true;
        this->m_alarm_latch = evt.contains("alarm_latch")?evt["alarm_latch"].toBool():true;
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
        float cap = 50;
        if(ja.contains("capacity")){
            cap = ja["capacity"].toDouble();
        }

        this->Stacks = ja["count"].toInt();
        int batPerStack = ja["baterries_per_stack"].toInt();
        int cellPerBat = ja["cells_per_battery"].toInt();
        int ntcPerBat = ja["ntcs_per_stack"].toInt();
        bool hv = ja["hv_module"].toBool();
        for(int i=0;i<Stacks;i++){
            BMS_Stack *s = new BMS_Stack;
            s->enableHVModule();
            s->groupID(i+1);
            s->sviDevice()->capacity(cap);
            for(int i=0;i<batPerStack;i++){
                BMS_BMUDevice *bat = new BMS_BMUDevice(cellPerBat,ntcPerBat);
                bat->deviceID(i+1);
                bat->ov_set(cell_ov_set);
                bat->ov_clr(cell_ov_clr);
                bat->uv_set(cell_uv_set);
                bat->uv_clr(cell_uv_clr);
                bat->ot_set(cell_ot_set);
                bat->ot_clr(cell_ot_clr);
                bat->ut_set(cell_ut_set);
                bat->ut_clr(cell_ut_clr);
                bat->duration(duration);
                s->addBattery(bat);
            }
            this->m_stacks.append(s);
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
                if(vs.contains("limit")){
                    QJsonArray a = vs["limit"].toArray();
                    if(a.size() == n){
                        for(int i=0;i<n;i++){
                            m_bcuDevice->vsource_limit(i,a[i].toInt());
                        }
                    }
                }
            }
        }
    }
//    if(ret){
//        generateSystemStructure();
//    }
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
        BMS_Stack *info = new BMS_Stack();
        quint8 id = 1; // bmu id start from 1
        for(int i=0;i<c->m_nofBatteries;i++){
            BMS_BMUDevice *bat = new BMS_BMUDevice(c->m_nofCellPerBattery,c->m_nofNTCPerBattery);
            bat->deviceID(id++);
            // signal connection

            info->addBattery(bat);
        }
        info->enableHVModule();
        info->groupID(c->m_groupID);

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
        foreach(BMS_Stack *s,m_stacks){
            s->feedData(identifier,data);
        }
    }
}

void BMS_System::startSimulator(int interval)
{
    m_simulateTimer = new QTimer();
    connect(m_simulateTimer,&QTimer::timeout,this,&BMS_System::simulate);
    m_simulateTimer->start(interval);
    m_simulating = true;
}

void BMS_System::stopSimulator()
{
    m_simulateTimer->stop();
    m_simulateTimer->deleteLater();
    m_simulating = false;
}

void BMS_System::simulate()
{
    //feed data to stacks
//    ushort sim_data[4];
//    QByteArray data;
//    QDataStream ds(&data, QIODevice::ReadWrite);

//    quint32 total_voltage = 0;

    foreach(BMS_Stack *s,m_stacks){
        s->simData();
    }


}

void BMS_System::enableAlarmSystem(bool en)
{
    if(en){
        if(m_validTimer == nullptr){
            m_validTimer = new QTimer;
            connect(m_validTimer,&QTimer::timeout,this,&BMS_System::validState);
        }
        m_validTimer->start(m_validInterval);
    }
    else{
        if(m_validTimer != nullptr && m_validTimer->isActive()){
            m_validTimer->stop();
        }
    }
}

quint32 BMS_System::alarmState()
{
    quint32 alarm = 0x0;
    foreach(BMS_Stack *s, m_stacks){
        alarm |= s->alarmState();
    }

    // check if bcu lost
    if(m_bcuDevice->deviceLost()){
        alarm |= bms::BCU_LOST;
    }
    // check if svi lost
    foreach (BMS_Stack *s, m_stacks) {
        if(s->sviDevice()->deviceLost()){
            alarm |= bms::BCU_LOST;
        }
        foreach (BMS_BMUDevice *b, s->batteries()) {
            if(b->deviceLost()){
                alarm |= bms::BMU_LOST;
            }
        }
    }
    return alarm;
}

void BMS_System::validState()
{
    QString msg;
    // check each battery in stacks and svi
    ushort minCellVoltage = 0xffff;
    foreach(BMS_Stack *s, m_stacks){
        // check if svi device lost

//        if(s->sviDevice()->lastSeen() > 5000){
//            emit deviceLost(GROUP(s->groupID()) | 0x31);
//        }
        int v = s->sviDevice()->voltageAlarm();
        if(s->sviDevice()->voltAlarm() && v == 0){
            msg = QString("第[%1]簇,過/欠壓[%2]V警報復歸").arg(s->groupID()).arg(s->stackVoltage());
            s->sviDevice()->voltAlarm(false);
            evt_log(msg);
        }
        else{
            if(v == 1){
                msg = QString("第[%1]簇,過壓[%2]V警報").arg(s->groupID()).arg(s->stackVoltage());
                s->sviDevice()->voltAlarm(true);
                evt_log(msg);
            }
            else if(v == -1){
                msg = QString("第[%1]簇,欠壓[%2]V警報").arg(s->groupID()).arg(s->stackVoltage());
                s->sviDevice()->voltAlarm(true);
                evt_log(msg);
            }
        }

        v = s->sviDevice()->currentAlarm();
        if(s->sviDevice()->ampereAlarm() && v==0){
            msg = QString("第[%1]簇,過/欠流[%2]V警報復歸").arg(s->groupID()).arg(s->stackCurrent());
            s->sviDevice()->ampereAlarm(true);
            evt_log(msg);
        }
        else{
            if(v == 1){
                msg = QString("第[%1]簇,過流[%2]V警報").arg(s->groupID()).arg(s->stackCurrent());
                s->sviDevice()->ampereAlarm(false);
                evt_log(msg);
            }
            else if(v == -1){
                msg = QString("第[%1]簇,欠流[%2]V警報").arg(s->groupID()).arg(s->stackCurrent());
                s->sviDevice()->ampereAlarm(false);
                evt_log(msg);
            }
        }

        quint16 set,clr,res,cmp;
        foreach(BMS_BMUDevice *b,s->batteries()){
            // check if bmu lost
            if(b->deviceLost() && !m_simulating){
                b->resetValues();
            }
            minCellVoltage = minCellVoltage > b->minCellVoltage()?b->minCellVoltage():minCellVoltage;
            if((res = b->ovState(&set,&clr)) != 0x00){
                quint16 mask = b->ovSetMask();
                for(int i=0;i<b->cellCount();i++){
                    cmp = (1 << i);
                    if((res & cmp)){
                        if((set & cmp)){
                            mask |= cmp;
                            msg = QString("第[%1]號電池,電芯[%2]過壓[%3]mV警報").arg(b->deviceID()).arg(i+1).arg(b->cellVoltage(i));
                        }
                        else if(clr & cmp){
                            mask &= ~cmp;
                            msg = QString("第[%1]號電池,電芯[%2]過壓警報清除[%3]mV").arg(b->deviceID()).arg(i+1).arg(b->cellVoltage(i));
                        }
                        evt_log(msg);
                    }
                }
                b->ovSetMask(mask);
            }
            if((res = b->uvState(&set,&clr)) != 0x00){
                quint16 mask = b->uvSetMask();
                for(int i=0;i<b->cellCount();i++){
                    cmp = (1 << i);
                    if((res & cmp)){
                        if((set & cmp)){
                            mask |= cmp;
                            msg = QString("第[%1]號電池,電芯[%2]欠壓[%3]mV警報").arg(b->deviceID()).arg(i+1).arg(b->cellVoltage(i));
                        }
                        else if(clr & cmp){
                            mask &= ~cmp;
                            msg = QString("第[%1]號電池,電芯[%2]欠壓警報清除[%3]mV").arg(b->deviceID()).arg(i+1).arg(b->cellVoltage(i));
                        }
                        evt_log(msg);
                    }
                }
                b->uvSetMask(mask);
            }
            if((res = b->otState(&set,&clr)) != 0x00){
                quint16 mask = b->otSetMask();
                for(int i=0;i<b->ntcCount();i++){
                    cmp = (1 << i);
                    if((res & cmp)){
                        if((set & cmp)){
                            mask |= cmp;
                            msg = QString("第[%1]號電池,度度[%2]過溫[%3]警報").arg(b->deviceID()).arg(i+1).arg(b->cellVoltage(i));
                        }
                        else if(clr & cmp){
                            mask &= ~cmp;
                            msg = QString("第[%1]號電池,溫度[%2]過溫警報清除[%3]mV").arg(b->deviceID()).arg(i+1).arg(b->cellVoltage(i));
                        }
                        evt_log(msg);
                    }
                }
                b->otSetMask(mask);
            }
            if((res = b->utState(&set,&clr)) != 0x00){
                quint16 mask = b->utSetMask();
                for(int i=0;i<b->ntcCount();i++){
                    cmp = (1 << i);
                    if((res & cmp)){
                        if((set & cmp)){
                            mask |= cmp;
                            msg = QString("第[%1]號電池,溫度[%2]低溫[%3]警報").arg(b->deviceID()).arg(i+1).arg(b->cellVoltage(i));
                        }
                        else if(clr & cmp){
                            mask &= ~cmp;
                            msg = QString("第[%1]號電池,溫度[%2]低溫警報清除[%3]").arg(b->deviceID()).arg(i+1).arg(b->cellVoltage(i));
                        }
                        evt_log(msg);
                    }
                }
                b->utSetMask(mask);
            }
        }
    }

    // set balancing voltage
    if(minCellVoltage > this->BalancingVoltage){
        // broadcast
        emit setBalancingVoltage(minCellVoltage);
    }
    else{
        minCellVoltage = this->BalancingVoltage;
    }

    m_cellMinVoltage = minCellVoltage;

}

QByteArray BMS_System::data()
{
    // loop through each stack to gather data
    QByteArray d;
    QDataStream s(&d,QIODevice::WriteOnly);
    foreach(BMS_Stack *b,m_stacks){
        s << b->data();
    }

    return d;
}


QDataStream& operator<<(QDataStream &out, const BMS_System *sys)
{
    out << sys->Stacks;
    out << sys->m_bcuDevice;
    foreach (BMS_Stack *s, sys->m_stacks) {
        out << s;
    }
    return out;
}

QDataStream& operator >> (QDataStream &in, BMS_System *sys)
{
    in >> sys->Stacks;
    if(sys->m_bcuDevice == nullptr){
        sys->m_bcuDevice = new BMS_BCUDevice();
    }
    in >> sys->m_bcuDevice;
    if(sys->m_stacks.size() == 0){
        for(quint8 i=0;i<sys->Stacks;i++){
            sys->m_stacks.append(new BMS_Stack());
        }
    }
    foreach (BMS_Stack *s, sys->m_stacks) {
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

void BMS_System::log(QByteArray data){
    if(!m_enableLog) return;
    //QString path = QString("%1/%2.bin").arg(m_logPath).arg(QDateTime::currentMSecsSinceEpoch(),16,16,QLatin1Char('0'));
    QString path = this->m_logPath + "/" + QDateTime::currentDateTime().toString("yyyyMMdd_hhmmss.bin");
    QFile f(path);
    if(f.open(QIODevice::WriteOnly)){
        QDataStream ds(&f);
        ds << data;
        f.close();
    }
    QDir dir(this->m_logPath);
    if(this->m_logRecords > 0){
        if(dir.count() > (this->m_logRecords+10)){
            QFileInfoList files = dir.entryInfoList(QDir::Files | QDir::NoDotAndDotDot, QDir::Time | QDir::Reversed);
            for(int i=0;i<files.size()-m_logRecords;i++){
//            for(int i=files.size();i>m_logRecords;--i){
                const QFileInfo& info = files.at(i);
                QFile::remove(info.absoluteFilePath());
            }
        }
    }
    else if(this->m_logDays > 0){

    }

}

void BMS_System::emg_log(QByteArray data){
    if(!m_enableLog) return;

}

void BMS_System::emg_log(QString msg)
{
    if(!m_enableLog) return;

}

void BMS_System::evt_log(QString msg)
{
    BMS_Event *evt = new BMS_Event;
    evt->m_description = msg;
    evt->m_timeStamp = QDateTime::currentDateTime();

    QString path = this->m_logPath + "/" + QDateTime::currentDateTime().toString("yyyyMMdd.log");
    QFile f(path);
    if(f.open(QIODevice::WriteOnly)){
        QTextStream ds(&f);
        ds << evt;
        f.close();
    }
}

void BMS_System::logPath(QString path){m_logPath = path;}

BMS_BCUDevice* BMS_System::bcu(){return m_bcuDevice;}

QList<int> BMS_System::batteriesPerStack()
{
    QList<int> ret;
    foreach(BMS_Stack *s,m_stacks){
        ret << s->BatteryCount();
    }
    if(ret.size() == 0){
        ret << 0;
    }
    return ret;
}

CAN_Packet *BMS_System::setBalancing(quint16 bv, quint8 bh, quint8 be, quint16 on, quint16 off)
{
    CAN_Packet *ret = nullptr;
    ret = new CAN_Packet();
    ret->Command = 0x132;
    QDataStream ds(&ret->data,QIODevice::WriteOnly);
    ds.setByteOrder(QDataStream::LittleEndian);
    quint16 u16;
    ds << bv;
    ds << bh;
    ds << be;
    u16 = on;
    if(u16 > 40){
        u16 = 40;
    }
    ds << u16;

    if(off < u16){
        ds << u16;
    }
    else{
        ds << off;
    }

    return ret;
}



bool BMS_System::enableLog()
{
    return m_enableLog;
}

void BMS_System::enableLog(bool enable)
{
    m_enableLog = enable;
}

int BMS_System::logDays()
{
    return m_logDays;
}
void BMS_System::logDays(int days)
{
    m_logDays = days;
}
int BMS_System::logRecords()
{
    return m_logRecords;
}
void BMS_System::logRecords(int recs)
{
    m_logRecords = recs;
}

void BMS_System::On_BMU_ov(quint16 mask)
{
//    BMS_BMUDevice *dev = (BMS_BMUDevice*)sender();
    BMS_BMUDevice *dev = this->m_stacks[0]->batteries().at(0);
    if(!dev->alarm()){
        dev->alarm(true);
        // add event log
        QString str = QString("第[%1]號電池 ").arg(dev->deviceID());
        for(int i=0;i<dev->cellCount();i++){
            if(mask & (1 << i)){
                str += QString("第(%1)顆電芯過壓 (%2)mV").arg(i+1).arg(dev->cellVoltage(i));
            }
        }
        this->evt_log(str);
    }
}

void BMS_System::On_BMU_uv(quint16 mask)
{

}

void BMS_System::On_BMU_ot(quint16 mask)
{

}

void BMS_System::On_BMU_ut(quint16 mask)
{

}

void BMS_System::On_SVI_ov()
{

}

void BMS_System::On_SVI_oc()
{

}

void BMS_System::clearAlarm()
{
    foreach(BMS_Stack *s, m_stacks){
        s->clearAlarm();
    }
}


CAN_Packet *BMS_System::broadcastBalancing()
{
    if(m_cellMinVoltage == m_currentBalanceVoltage) return nullptr;
    if(m_cellMinVoltage < BalancingVoltage) return nullptr;

    CAN_Packet *ret = new CAN_Packet;
    ret->Command = 0x080;
    ret->remote = false;
    QDataStream ds(&ret->data,QIODevice::WriteOnly);
    ds.setByteOrder(QDataStream::LittleEndian);
    ds << m_cellMinVoltage;
    m_currentBalanceVoltage = m_cellMinVoltage;
    return ret;

}

CAN_Packet *BMS_System::startBMUs(bool enable)
{
    CAN_Packet *ret = new CAN_Packet;
    ret->Command = 0x081;
    ret->remote = false;
    QDataStream ds(&ret->data,QIODevice::WriteOnly);
    ds.setByteOrder(QDataStream::LittleEndian);
    quint8 en = enable?1:0;
    ds << en;
    en = 0; ds << en;
    quint16 reportMS = (quint16)normalReportMS;
    ds << reportMS;
    return ret;
}

CAN_Packet *BMS_System::heartBeat()
{
    CAN_Packet *ret = new CAN_Packet;
    ret->Command = 0x020;
    ret->remote = true;
}

