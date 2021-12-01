#include "bms_system.h"
#include "bms_def.h"
#include "bms_bmudevice.h"
#include "bms_bcudevice.h"
#include "bms_svidevice.h"
#include "bms_stack.h"
#include "bms_localconfig.h"
#include <QStorageInfo>

BMS_System::BMS_System(QObject *parent) : QObject(parent)
{
    m_startTime = QDateTime::currentDateTime();
    m_localConfig = new BMS_LocalConfig;

}

BMS_System::~BMS_System(){
   m_localConfig->deleteLater();
   foreach (BMS_Stack *s, m_stacks) {
       s->deleteLater();
       s = nullptr;
   }
   m_bcuDevice->deleteLater();
   m_bcuDevice = nullptr;
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

void BMS_System::log(QString msg)
{
    QString logText = QString("BMS:%1").arg(msg);
    emit logMessage(logText);
}

void BMS_System::generateSystemStructure()
{
    // test log path
    QString p;
    if(QSysInfo::productType().contains("win")){
        p = "d:/temp/bms/log";
    }
    else{
        p = m_localConfig->record.LogPath;
    }
    if(!QDir(p).exists()){
        QDir().mkdir(p);
    }
    QString recPath = p + "/record";
    if(!QDir(recPath).exists()){
        QDir().mkdir(recPath);
    }

    m_logPath = p;
    if(m_localConfig->system.ConfigReady){
       BalancingVoltage = m_localConfig->balancing.BalancingVolt.toDouble()*1000;
       BalancingHystersis = m_localConfig->balancing.HystersisMV.toInt();
       BalancingOnTime = m_localConfig->balancing.On_TimeSec.toInt();
       BalancingOffTime = m_localConfig->balancing.Off_TimeSec.toInt();
       m_balancingCurrentThes = (int)(m_localConfig->system.BalancingThreshold.toFloat()*10);
    }

    //todo: report time

    //todo: event physic output mapping

    // create stacks
    if(m_localConfig->stack.ConfigReady){
        int stacks = m_localConfig->stack.StackCount.toInt();
        float cap = m_localConfig->stack.Capacity.toDouble();
        int bps = m_localConfig->stack.BatteryPerStack.toInt();
        int cpb = m_localConfig->stack.CellPerBattery.toInt();
        int npb = m_localConfig->stack.NTCPerBattery.toInt();
        BMS_CriteriaConfig *sc = &m_localConfig->criteria.stack;
        BMS_CriteriaSOC *soc = &m_localConfig->criteria.soc;
        BMS_CriteriaConfig *cc = &m_localConfig->criteria.cell;
        for(int i=0;i<stacks;i++){
            BMS_Stack *s = new BMS_Stack;
            int val;
            s->enableHVModule();
            val = m_localConfig->system.SVA_ValidInterval.toInt();
            if(val < 5) val = 5;
            s->svaValidInterval(val);
            //qDebug()<<"Set sva valid interval:"<<val;
            val = m_localConfig->system.BMU_ValidInterval.toInt();
            if(val < 5) val = 5;
            //qDebug()<<"Set bmu valid interval:"<<val;
            s->bmuValidInterval(val);
            s->sviDevice()->setSVWarningHighPair(sc->volt_warning.High_Set.toDouble()*10,sc->volt_warning.High_Clr.toDouble()*10,sc->volt_warning.Duration.toInt());
            s->sviDevice()->setSVWarningLowPair(sc->volt_warning.Low_Set.toDouble()*10,sc->volt_warning.Low_Set.toDouble()*10,sc->volt_warning.Duration.toInt());
            s->sviDevice()->setSVAlarmHighPair(sc->volt_alarm.High_Set.toDouble()*10,sc->volt_alarm.High_Clr.toDouble()*10,sc->volt_alarm.Duration.toInt());
            s->sviDevice()->setSVAlarmLowPair(sc->volt_alarm.Low_Set.toDouble()*10 ,sc->volt_alarm.Low_Set.toDouble()*10 ,sc->volt_alarm.Duration.toInt());


            s->sviDevice()->setSOCWarningLowPair(soc->warning.Low_Set.toDouble(),soc->warning.Low_Clr.toDouble(),soc->warning.Duration.toInt());
            s->sviDevice()->setSOCAlarmLowPair(soc->alarm.Low_Set.toDouble(),soc->alarm.Low_Clr.toDouble(),soc->alarm.Duration.toInt());

            s->groupID(i+1);
            s->sviDevice()->capacity(cap);
            for(int i=0;i<bps;i++){
                BMS_BMUDevice *bat = new BMS_BMUDevice(cpb,npb);
                bat->deviceID(i+1);
                bat->setCVWarningHighPair((short)(cc->volt_warning.High_Set.toDouble()*1000),(short)(cc->volt_warning.High_Clr.toDouble()*1000),cc->volt_warning.Duration.toInt());
                bat->setCVWarningLowPair( (short)(cc->volt_warning.Low_Set.toDouble()*1000),(short)(cc->volt_warning.Low_Clr.toDouble()*1000),cc->volt_warning.Duration.toInt());
                bat->setCVAlarmHighPair((short)(cc->volt_alarm.High_Set.toDouble()*1000),(short)(cc->volt_alarm.High_Clr.toDouble()*1000),cc->volt_alarm.Duration.toInt());
                bat->setCVAlarmLowPair( (short)(cc->volt_alarm.Low_Set.toDouble()*1000), (short)(cc->volt_alarm.Low_Clr.toDouble()*1000) ,cc->volt_alarm.Duration.toInt());
                bat->setCTWarningHighPair((short)(cc->temp_warning.High_Set.toDouble()*10),(short)(cc->temp_warning.High_Clr.toDouble()*10),cc->temp_warning.Duration.toInt());
                bat->setCTWarningLowPair( (short)(cc->temp_warning.Low_Set.toDouble()*10), (short)(cc->temp_warning.Low_Clr.toDouble()*10) ,cc->temp_warning.Duration.toInt());
                bat->setCTAlarmHighPair((short)(cc->temp_alarm.High_Set.toDouble()*10),(short)(cc->temp_alarm.High_Clr.toDouble()*10),cc->temp_alarm.Duration.toInt());
                bat->setCTAlarmLowPair( (short)(cc->temp_alarm.Low_Set.toDouble()*10), (short)(cc->temp_alarm.Low_Clr.toDouble()*10) ,cc->temp_alarm.Duration.toInt());
                s->addBattery(bat);
            }
            this->m_stacks.append(s);

        }

        // add bcu
        m_bcuDevice = new BMS_BCUDevice();
        m_bcuDevice->add_digital_input(2);
        m_bcuDevice->add_digital_output(2);
        m_bcuDevice->add_analog_input(8);
        m_bcuDevice->add_voltage_source(2);
        m_bcuDevice->add_pwm_in(2);
        m_bcuDevice->vsource_limit(0,3000);
        m_bcuDevice->vsource_limit(1,3000);
    }

    if(m_localConfig->system.ConfigReady){
        this->Alias = m_localConfig->system.Alias;
        this->connectionString = m_localConfig->system.HostIP;
        this->connectionPort = m_localConfig->system.ListenPort.toInt();
        this->m_validInterval = m_localConfig->system.ValidInterval.toInt();
        this->m_enableLog = true;
        this->m_useSimulator = m_localConfig->system.Simulate;
    }
    if(m_localConfig->record.ConfigReady){
       quint16 n = m_localConfig->record.LogEventCount.toInt()+50;
       this->m_maxEvents = n;
       n = m_localConfig->record.EventRecordCount.toInt()+50;
       this->m_maxEventRecords = n;
    }

    if(m_localConfig->event_output.ConfigReady){
        m_warning_out_id = m_localConfig->event_output.warning_out.toInt();
        m_warning_latch = m_localConfig->event_output.warning_latch;
        m_alarm_out_id = m_localConfig->event_output.alarm_out.toInt();
        m_alarm_latch = m_localConfig->event_output.alarm_latch;
    }

    // load soc if exist
    QString socPath;
    if(QSysInfo::productType().contains("win")){
        socPath = "d:/temp/bms/soc";
    }
    else{
        socPath = "/opt/bms/soc";
    }

    QFile f(socPath);
    if(f.exists()){
        f.open(QIODevice::ReadOnly);
        QTextStream ts(&f);
        int r = 0;
        while(!ts.atEnd()){
            double soc = ts.readLine().trimmed().toDouble();
            if(r < m_stacks.size()){
                m_stacks[r]->sviDevice()->soc(soc);
            }
        }
    }
    checkDiskSpace();
}

bool BMS_System::Configuration2(QByteArray b)
{
    m_localConfig->load(b);
    generateSystemStructure();

    return true;
}

bool BMS_System::Configuration2(QString path)
{
    m_localConfig->load(path);
    generateSystemStructure();

//    // test log path
//    QString p;
//    if(QSysInfo::productType().contains("win")){
//        p = "d:/temp/bms/log";
//    }
//    else{
//        p = m_localConfig->record.LogPath;
//    }
//    m_logPath = p;

//    if(!QDir(p).exists()){
//        QDir().mkdir(p);
//    }
//    if(m_localConfig->system.ConfigReady){
//       BalancingVoltage = m_localConfig->balancing.BalancingVolt.toDouble()*1000;
//       BalancingHystersis = m_localConfig->balancing.HystersisMV.toInt();
//       BalancingOnTime = m_localConfig->balancing.On_TimeSec.toInt();
//       BalancingOffTime = m_localConfig->balancing.Off_TimeSec.toInt();
//    }

//    //todo: report time

//    //todo: event physic output mapping

//    // create stacks
//    if(m_localConfig->stack.ConfigReady){
//        int stacks = m_localConfig->stack.StackCount.toInt();
//        float cap = m_localConfig->stack.Capacity.toDouble();
//        int bps = m_localConfig->stack.BatteryPerStack.toInt();
//        int cpb = m_localConfig->stack.CellPerBattery.toInt();
//        int npb = m_localConfig->stack.NTCPerBattery.toInt();
//        BMS_CriteriaConfig *sc = &m_localConfig->criteria.stack;
//        BMS_CriteriaSOC *soc = &m_localConfig->criteria.soc;
//        BMS_CriteriaConfig *cc = &m_localConfig->criteria.cell;
//        for(int i=0;i<stacks;i++){
//            BMS_Stack *s = new BMS_Stack;
//            s->enableHVModule();
//            s->sviDevice()->setSVWarningHighPair(sc->volt_warning.High_Set.toDouble()*10,sc->volt_warning.High_Clr.toDouble()*10,sc->volt_warning.Duration.toInt());
//            s->sviDevice()->setSVWarningLowPair(sc->volt_warning.Low_Set.toDouble()*10,sc->volt_warning.Low_Set.toDouble()*10,sc->volt_warning.Duration.toInt());
//            s->sviDevice()->setSVAlarmHighPair(sc->volt_alarm.High_Set.toDouble()*10,sc->volt_alarm.High_Clr.toDouble()*10,sc->volt_alarm.Duration.toInt());
//            s->sviDevice()->setSVAlarmLowPair(sc->volt_alarm.Low_Set.toDouble()*10 ,sc->volt_alarm.Low_Set.toDouble()*10 ,sc->volt_alarm.Duration.toInt());

//            s->sviDevice()->setSOCWarningLowPair(soc->warning.Low_Set.toDouble(),soc->warning.Low_Clr.toDouble(),soc->warning.Duration.toInt());
//            s->sviDevice()->setSOCAlarmLowPair(soc->alarm.Low_Set.toDouble(),soc->alarm.Low_Clr.toDouble(),soc->alarm.Duration.toInt());

//            s->groupID(i+1);
//            s->sviDevice()->capacity(cap);
//            for(int i=0;i<bps;i++){
//                BMS_BMUDevice *bat = new BMS_BMUDevice(cpb,npb);
//                bat->deviceID(i+1);
//                bat->setCVWarningHighPair((short)(cc->volt_warning.High_Set.toDouble()*1000),(short)(cc->volt_warning.High_Clr.toDouble()*1000),cc->volt_warning.Duration.toInt());
//                bat->setCVWarningLowPair( (short)(cc->volt_warning.Low_Set.toDouble()*1000),(short)(cc->volt_warning.Low_Clr.toDouble()*1000),cc->volt_warning.Duration.toInt());
//                bat->setCVAlarmHighPair((short)(cc->volt_alarm.High_Set.toDouble()*1000),(short)(cc->volt_alarm.High_Clr.toDouble()*1000),cc->volt_alarm.Duration.toInt());
//                bat->setCVAlarmLowPair( (short)(cc->volt_alarm.Low_Set.toDouble()*1000), (short)(cc->volt_alarm.Low_Clr.toDouble()*1000) ,cc->volt_alarm.Duration.toInt());
//                bat->setCTWarningHighPair((short)(cc->temp_warning.High_Set.toDouble()*10),(short)(cc->temp_warning.High_Clr.toDouble()*10),cc->temp_warning.Duration.toInt());
//                bat->setCTWarningLowPair( (short)(cc->temp_warning.Low_Set.toDouble()*10), (short)(cc->temp_warning.Low_Clr.toDouble()*10) ,cc->temp_warning.Duration.toInt());
//                bat->setCTAlarmHighPair((short)(cc->temp_alarm.High_Set.toDouble()*10),(short)(cc->temp_alarm.High_Clr.toDouble()*10),cc->temp_alarm.Duration.toInt());
//                bat->setCTAlarmLowPair( (short)(cc->temp_alarm.Low_Set.toDouble()*10), (short)(cc->temp_alarm.Low_Clr.toDouble()*10) ,cc->temp_alarm.Duration.toInt());
//                s->addBattery(bat);
//            }
//            this->m_stacks.append(s);

//        }

//        // add bcu
//        m_bcuDevice = new BMS_BCUDevice();
//        m_bcuDevice->add_digital_input(2);
//        m_bcuDevice->add_digital_output(2);
//        m_bcuDevice->add_analog_input(8);
//        m_bcuDevice->add_voltage_source(2);
//        m_bcuDevice->add_pwm_in(2);
//    }

//    if(m_localConfig->system.ConfigReady){
//        this->Alias = m_localConfig->system.Alias;
//        this->connectionString = m_localConfig->system.HostIP;
//        this->connectionPort = m_localConfig->system.ListenPort.toInt();
//        this->m_validInterval = m_localConfig->system.ValidInterval.toInt();
//        this->m_enableLog = true;
//        this->m_useSimulator = m_localConfig->system.Simulate;
//    }

//    if(m_localConfig->event_output.ConfigReady){
//        m_warning_out_id = m_localConfig->event_output.warning_out.toInt();
//        m_warning_latch = m_localConfig->event_output.warning_latch;
//        m_alarm_out_id = m_localConfig->event_output.alarm_out.toInt();
//        m_alarm_latch = m_localConfig->event_output.alarm_latch;
//    }

    return true;
}
bool BMS_System::Configuration(QByteArray data)
{
    bool ret = false;
    QJsonParseError e;
    QJsonDocument d = QJsonDocument::fromJson(data,&e);
    if(d.isNull()){

        qDebug()<<"Wrong configuration file";
        log("Wrong configuration file");
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
        QString tmpFile = this->m_logPath + "/test.bin";
        QFile f(tmpFile);
        if(f.open(QIODevice::WriteOnly)){
            QFileInfo info(f);
            //qDebug()<<"Abs path"<<info.absoluteFilePath();
            this->m_logPath = info.absolutePath();
        }
        f.close();
        f.remove();
    }
    else{
        this->m_logPath = QCoreApplication::applicationDirPath()+ "/" + path;
    }
    this->m_logDays = obj.contains("log_days")?obj["log_days"].toInt():-1;
    this->m_logRecords= obj.contains("log_records")?obj["log_records"].toInt():200;

    QString sysPath = this->m_logPath + "/sys";
    // check if folder presents
    if(!QDir(sysPath).exists()){
        QDir().mkdir(sysPath);
    }


    if(obj.contains("balancing")){
        QJsonObject bal = obj["balancing"].toObject();
        this->BalancingVoltage = bal["voltage"].toInt();
        this->BalancingHystersis = bal["hystersis"].toInt();
        this->BalancingOnTime = bal["on_time"].toInt();
        this->BalancingOffTime = bal["off_time"].toInt();
    }

    if(obj.contains("report")){
        qDebug()<<"report section present";
        QJsonObject rpt = obj["report"].toObject();
        this->normalReportMS = rpt["normal"].toInt();
        this->errorReportMS = rpt["error"].toInt();
    }
    else{
        qDebug()<<"report section NOT present";
        this->normalReportMS = 1000;
        this->errorReportMS = 100;
    }

    // for warning values
    quint16 cell_ot_set = 0, cell_ot_clr = 0;
    quint16 cell_ut_set = 0, cell_ut_clr = 0;
    quint16 cell_warning_dt = 5;
    quint16 cell_ov_set = 0, cell_ov_clr = 0;
    quint16 cell_uv_set = 0, cell_uv_clr = 0;
    quint16 cell_warning_dv = 5;
    quint16 stack_ov_set = 0, stack_ov_clr = 0;
    quint16 stack_uv_set = 0, stack_uv_clr = 0;
    quint16 stack_warning_dv = 5;
    quint16 stack_oc_set = 0, stack_oc_clr = 0;
    quint16 stack_uc_set = 0, stack_uc_clr = 0;
    quint16 stack_warning_dc = 5;

    // for alarm values
    quint16 cell_ota_set = 0, cell_ota_clr = 0;
    quint16 cell_uta_set = 0, cell_uta_clr = 0;
    quint16 cell_alarm_dt = 5;
    quint16 cell_ova_set = 0, cell_ova_clr = 0;
    quint16 cell_uva_set = 0, cell_uva_clr = 0;
    quint16 cell_alarm_dv = 5;
    quint16 stack_ova_set = 0, stack_ova_clr = 0;
    quint16 stack_uva_set = 0, stack_uva_clr = 0;
    quint16 stack_alarm_dv = 5;
    quint16 stack_oca_set = 0, stack_oca_clr = 0;
    quint16 stack_uca_set = 0, stack_uca_clr = 0;
    quint16 stack_alarm_dc = 5;

    quint16 soc_uv_waning_set = 0, soc_uv_warning_clr = 0;
    quint16 soc_uv_alarm_set = 0, soc_uv_alarm_clr = 0;
    quint16 soc_uv_warning_dt, soc_uv_alarm_dt;

    if(obj.contains("criteria")){
        QJsonObject crit = obj["criteria"].toObject();
        if(crit.contains("cell")){
            QJsonObject cob = crit["cell"].toObject();
            if(cob.contains("volt-warning")){
                QJsonObject cw = cob["volt-warning"].toObject();
                if(cw.contains("high_set")) cell_ov_set = (ushort)(cw["high_set"].toString().trimmed().toDouble()*1000);
                if(cw.contains("high_clr")) cell_ov_clr = (ushort)(cw["high_clr"].toString().trimmed().toDouble()*1000);
                if(cw.contains("low_set")) cell_uv_set = (ushort)(cw["low_set"].toString().trimmed().toDouble()*1000);
                if(cw.contains("low_clr")) cell_uv_clr = (ushort)(cw["low_clr"].toString().trimmed().toDouble()*1000);
                if(cw.contains("duration")) cell_warning_dv = (ushort)cw["duration"].toString().trimmed().toInt();
            }
            if(cob.contains("temp-warning")){
                QJsonObject cw = cob["temp-warning"].toObject();
                if(cw.contains("high_set")) cell_ot_set = (ushort)(cw["high_set"].toString().trimmed().toDouble()*10);
                if(cw.contains("high_clr")) cell_ot_clr = (ushort)(cw["high_clr"].toString().trimmed().toDouble()*10);
                if(cw.contains("low_set")) cell_ut_set = (ushort)(cw["low_set"].toString().trimmed().toDouble()*10);
                if(cw.contains("low_clr")) cell_ut_clr = (ushort)(cw["low_clr"].toString().trimmed().toDouble()*10);
                if(cw.contains("duration")) cell_warning_dt = (ushort)cw["duration"].toString().trimmed().toInt();
            }
            if(cob.contains("volt-alarm")){
                QJsonObject cw = cob["volt-alarm"].toObject();
                if(cw.contains("high_set")) cell_ova_set = (ushort)(cw["high_set"].toString().trimmed().toDouble()*1000);
                if(cw.contains("high_clr")) cell_ova_clr = (ushort)(cw["high_clr"].toString().trimmed().toDouble()*1000);
                if(cw.contains("low_set")) cell_uva_set = (ushort)(cw["low_set"].toString().trimmed().toDouble()*1000);
                if(cw.contains("low_clr")) cell_uva_clr = (ushort)(cw["low_clr"].toString().trimmed().toDouble()*1000);
                if(cw.contains("duration")) cell_alarm_dv = (ushort)cw["duration"].toString().trimmed().toInt();
            }
            if(cob.contains("temp-alarm")){
                QJsonObject cw = cob["temp-alarm"].toObject();
                if(cw.contains("high_set")) cell_ota_set = (ushort)(cw["high_set"].toString().trimmed().toDouble()*10);
                if(cw.contains("high_clr")) cell_ota_clr = (ushort)(cw["high_clr"].toString().trimmed().toDouble()*10);
                if(cw.contains("low_set")) cell_uta_set = (ushort)(cw["low_set"].toString().trimmed().toDouble()*10);
                if(cw.contains("low_clr")) cell_uta_clr = (ushort)(cw["low_clr"].toString().trimmed().toDouble()*10);
                if(cw.contains("duration")) cell_alarm_dt = (ushort)cw["duration"].toString().trimmed().toInt();
            }
        }
        if(crit.contains("stack")){
            QJsonObject sob = crit["stack"].toObject();
            if(sob.contains("volt-warning")){
                QJsonObject cw = sob["volt-warning"].toObject();
                if(cw.contains("high_set")) stack_ov_set = (ushort)(cw["high_set"].toString().trimmed().toDouble()*10);
                if(cw.contains("high_clr")) stack_ov_clr = (ushort)(cw["high_clr"].toString().trimmed().toDouble()*10);
                if(cw.contains("low_set")) stack_uv_set = (ushort)(cw["low_set"].toString().trimmed().toDouble()*10);
                if(cw.contains("low_clr")) stack_uv_clr = (ushort)(cw["low_clr"].toString().trimmed().toDouble()*10);
                if(cw.contains("duration")) stack_warning_dv = (ushort)cw["duration"].toString().trimmed().toInt();
            }
            if(sob.contains("temp-warning")){
                QJsonObject cw = sob["temp-warning"].toObject();
                if(cw.contains("high_set")) stack_oca_set = (ushort)(cw["high_set"].toString().trimmed().toDouble()*10);
                if(cw.contains("high_clr")) stack_oca_clr = (ushort)(cw["high_clr"].toString().trimmed().toDouble()*10);
                if(cw.contains("duration")) stack_warning_dc = (ushort)cw["duration"].toString().trimmed().toInt();
            }
            if(sob.contains("volt-alarm")){
                QJsonObject cw = sob["volt-alarm"].toObject();
                if(cw.contains("high_set")) stack_ova_set = (ushort)(cw["high_set"].toString().trimmed().toDouble()*10);
                if(cw.contains("high_clr")) stack_ova_clr = (ushort)(cw["high_clr"].toString().trimmed().toDouble()*10);
                if(cw.contains("low_set")) stack_uva_set = (ushort)(cw["low_set"].toString().trimmed().toDouble()*10);
                if(cw.contains("low_clr")) stack_uva_clr = (ushort)(cw["low_clr"].toString().trimmed().toDouble()*10);
                if(cw.contains("duration")) stack_alarm_dv = (ushort)cw["duration"].toString().trimmed().toInt();
            }
            if(sob.contains("temp-alarm")){
                QJsonObject cw = sob["temp-alarm"].toObject();
                if(cw.contains("high_set")) stack_oca_set = (ushort)(cw["high_set"].toString().trimmed().toDouble()*10);
                if(cw.contains("high_clr")) stack_oca_clr = (ushort)(cw["high_clr"].toString().trimmed().toDouble()*10);
                if(cw.contains("duration")) stack_alarm_dc = (ushort)cw["duration"].toString().trimmed().toInt();
            }
        }

        if(crit.contains("soc")){
            QJsonObject sob = crit["soc"].toObject();
            if(sob.contains("warning")){
                QJsonObject cw = sob["warning"].toObject();
                if(cw.contains("low_set")) soc_uv_alarm_set = (ushort)(cw["low_set"].toString().trimmed().toDouble());
                if(cw.contains("low_clr")) soc_uv_alarm_clr = (ushort)(cw["low_clr"].toString().trimmed().toDouble());
                if(cw.contains("duration")) soc_uv_warning_dt = (ushort)cw["duration"].toString().trimmed().toInt();
            }
            if(sob.contains("alarm")){
                QJsonObject cw = sob["alarm"].toObject();
                if(cw.contains("low_set"))  soc_uv_alarm_set = (ushort)(cw["low_set"].toString().trimmed().toDouble());
                if(cw.contains("low_clr"))  soc_uv_alarm_clr = (ushort)(cw["low_clr"].toString().trimmed().toDouble());
                if(cw.contains("duration")) soc_uv_alarm_dt = (ushort)cw["duration"].toString().trimmed().toInt();
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
//        m_stackConfig.clear();
//        QJsonArray ja = obj["stacks"].toArray();
//        Stacks = 0;
//        foreach (QJsonValue o, ja) {
//            BMS_StackConfig *cfg = new BMS_StackConfig();
//            cfg->m_nofBatteries = o["baterries_per_stack"].toInt();
//            cfg->m_nofCellPerBattery = o["cells_per_battery"].toInt();
//            cfg->m_nofNTCPerBattery = o["ntcs_per_stack"].toInt();
//            cfg->m_hvboard = o["hv_module"].toBool();
//            cfg->m_groupID = o["group_id"].toInt();

//            m_stackConfig.append(cfg);
//            Stacks++;
//        }
//        ret = true;
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
            s->sviDevice()->ovWarningSet(stack_ov_set);
            s->sviDevice()->ovWarningClr(stack_ov_clr);
            s->sviDevice()->ovWarningEn(true);
            s->sviDevice()->uvWarningSet(stack_uv_set);
            s->sviDevice()->uvWarningClr(stack_uv_clr);
            s->sviDevice()->vWarningDuration(stack_warning_dv);
            s->sviDevice()->uvWarningEn(true);

            s->sviDevice()->ovAlarmSet(stack_ova_set);
            s->sviDevice()->ovAlarmClr(stack_ova_clr);
            s->sviDevice()->ovAlarmEn(true);
            s->sviDevice()->uvAlarmSet(stack_uva_set);
            s->sviDevice()->uvAlarmClr(stack_uva_clr);
            s->sviDevice()->vAlarmDuration(stack_alarm_dv);
            s->sviDevice()->uvAlarmEn(true);

            s->sviDevice()->setSVWarningHighPair(stack_ov_set,stack_ov_clr,stack_warning_dv);
            s->sviDevice()->setSVWarningLowPair(stack_uv_set,stack_uv_clr,stack_warning_dv);
            s->sviDevice()->setSVAlarmHighPair(stack_ova_set,stack_ova_clr,stack_alarm_dv);
            s->sviDevice()->setSVAlarmLowPair(stack_uva_set,stack_uva_clr,stack_alarm_dv);

            s->sviDevice()->setSOCWarningLowPair(soc_uv_waning_set,soc_uv_warning_clr,soc_uv_warning_dt);
            s->sviDevice()->setSOCAlarmLowPair(soc_uv_alarm_set,soc_uv_alarm_clr,soc_uv_alarm_dt);

            s->groupID(i+1);
            s->sviDevice()->capacity(cap);
            for(int i=0;i<batPerStack;i++){
                BMS_BMUDevice *bat = new BMS_BMUDevice(cellPerBat,ntcPerBat);
                bat->deviceID(i+1);
//                bat->ov_set(cell_ov_set);
//                bat->ov_clr(cell_ov_clr);
//                bat->uv_set(cell_uv_set);
//                bat->uv_clr(cell_uv_clr);
//                bat->ot_set(cell_ot_set);
//                bat->ot_clr(cell_ot_clr);
//                bat->ut_set(cell_ut_set);
//                bat->ut_clr(cell_ut_clr);
                //bat->duration(duration);
                bat->setCVWarningHighPair(cell_ov_set,cell_ov_clr,cell_warning_dv);
                bat->setCVWarningLowPair(cell_uv_set,cell_uv_clr,cell_warning_dv);
                bat->setCVAlarmHighPair(cell_ova_set,cell_ova_clr,cell_warning_dv);
                bat->setCVAlarmLowPair(cell_uva_set,cell_uva_clr,cell_warning_dv);
                bat->setCTWarningHighPair(cell_ot_set,cell_ot_clr,cell_warning_dt);
                bat->setCTWarningLowPair(cell_ut_set,cell_ut_clr,cell_warning_dt);
                bat->setCTAlarmHighPair(cell_ota_set,cell_ota_clr,cell_warning_dt);
                bat->setCTAlarmLowPair(cell_uta_set,cell_uta_clr,cell_warning_dt);
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
    return ret;
}

QByteArray BMS_System::Configuration()
{
    QByteArray ret;

    return ret;
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
    foreach(BMS_Stack *s,m_stacks){
        s->simData();
    }
    m_bcuDevice->simData();
}

void BMS_System::enableAlarmSystem(bool en)
{
    if(en){
        if(m_validTimer == nullptr){
            m_validTimer = new QTimer;
            connect(m_validTimer,&QTimer::timeout,this,&BMS_System::validState);
        }
        m_validTimer->start(m_validInterval);
        log("Alarm system enabled");
    }
    else{
        if(m_validTimer != nullptr && m_validTimer->isActive()){
            m_validTimer->stop();
        }
        log("Alarm system disabled");
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
        alarm |= (1 << bms::BCU_LOST);
    }

    return alarm;
}

void BMS_System::validState()
{
    QString msg;
    // check each battery in stacks and svi
    ushort minCellVoltage = 0xffff;
    foreach(BMS_Stack *s, m_stacks){
        s->valid();

        // add SVI/BMU lost record
        if(s->sviDevice()->isLosted()){
            if(!s->sviDevice()->deviceLost()){
                s->sviDevice()->isLosted(false);
                evt_log("SVI Recover","NO",s->state(),QString("S-%1").arg(s->groupID()));
            }
        }
        else{
            if(s->sviDevice()->deviceLost()){
                s->sviDevice()->isLosted(true);
                evt_log("SVI Lost","NO",s->state(),QString("S-%1").arg(s->groupID()));
            }
        }

        switch(s->sviDevice()->ovWarning()){
        case 1:
            evt_log("總壓過壓","一級",s->state(),QString("S-%1,%2V").arg(s->groupID()).arg((double)s->stackVoltage()/10));
            break;
        case 2:
            evt_log("總壓過壓復歸","一級",s->state(),QString("S-%1,%2V").arg(s->groupID()).arg((double)s->stackVoltage()/10));
            break;
        default:break;
        }

        switch(s->sviDevice()->ovAlarm()){
        case 1:
            evt_log("總壓過壓","二級",s->state(),QString("S-%1,%2V").arg(s->groupID()).arg((double)s->stackVoltage()/10));
            break;
        case 2:
            evt_log("總壓過壓復歸","二級",s->state(),QString("S-%1,%2V").arg(s->groupID()).arg((double)s->stackVoltage()/10));
            break;
        default:break;
        }

        switch(s->sviDevice()->uvWarning()){
        case 1:
            evt_log("總壓欠壓","一級",s->state(),QString("S-%1,%2V").arg(s->groupID()).arg((double)s->stackVoltage()/10));
            break;
        case 2:
            evt_log("總壓欠壓復歸","一級",s->state(),QString("S-%1,%2V").arg(s->groupID()).arg((double)s->stackVoltage()/10));
            break;
        default:break;
        }

        switch(s->sviDevice()->uvAlarm()){
        case 1:
            evt_log("總壓欠壓","二級",s->state(),QString("S-%1,%2V").arg(s->groupID()).arg((double)s->stackVoltage()/10));
            break;
        case 2:
            evt_log("總壓欠壓復歸","二級",s->state(),QString("S-%1,%2V").arg(s->groupID()).arg((double)s->stackVoltage()/10));
            break;
        default:break;
        }

        quint16 set,clr,res,cmp;
        foreach(BMS_BMUDevice *b,s->batteries()){
            // check if bmu lost
            if(b->isLosted()){
                if(!b->deviceLost()){
                    b->isLosted(false);
                    evt_log("BMU Recover","一級",s->state(),QString("S%1-B%2").arg(s->groupID()).arg(b->deviceID()));
                }
            }
            else{
                if(b->deviceLost() && !m_simulating){
                    b->isLosted(true);
                    b->resetValues();
                    evt_log("BMU Lost","一級",s->state(),QString("S%1-B%2").arg(s->groupID()).arg(b->deviceID()));
                }
            }
            minCellVoltage = minCellVoltage > b->minCellVoltage()?b->minCellVoltage():minCellVoltage;
            //if((res = b->ovState(&set,&clr)) != 0x00){
            if((res = b->cvWarningOVState(&set,&clr)) != 0x00){ // over voltage warning
                quint16 mask = b->cvWarningOVMask();
                for(int i=0;i<b->cellCount();i++){
                    cmp = (1 << i);
                    if((res & cmp)){
                        if((set & cmp)){
                            mask |= cmp;
//                            msg = QString("第[%1]號電池 電芯[%2]過壓[%3]mV警告").arg(b->deviceID()).arg(i+1).arg((double)b->cellVoltage(i)/1000);
                            //qDebug()<<"1";
                            evt_log("電芯過壓","一級",s->state(),QString("S%1-B%2-C%3 %4V").arg(s->groupID()).arg(b->deviceID()).arg(i+1).arg((double)b->cellVoltage(i)/1000));
                        }
                        else if(clr & cmp){
                            mask &= ~cmp;
                            //qDebug()<<"2";
                            evt_log("電芯過壓復歸","一級",s->state(),QString("S%1-B%2-C%3 %4V").arg(s->groupID()).arg(b->deviceID()).arg(i+1).arg((double)b->cellVoltage(i)/1000));
                        }
                    }
                }
                b->cvWarningOVHandled(mask);
            }

            if((res = b->cvAlarmOVState(&set,&clr)) != 0x00){ // over voltage alarm
                quint16 mask = b->cvAlarmOVMask();
                for(int i=0;i<b->cellCount();i++){
                    cmp = (1 << i);
                    if((res & cmp)){
                        if((set & cmp)){
                            mask |= cmp;
                            //qDebug()<<"2";
                            evt_log("電芯過壓","二級",s->state(),QString("S%1-B%2-C%3 %4V").arg(s->groupID()).arg(b->deviceID()).arg(i+1).arg((double)b->cellVoltage(i)/1000));
                        }
                        else if(clr & cmp){
                            mask &= ~cmp;
                            //qDebug()<<"4";
                            evt_log("電芯過壓復歸","二級",s->state(),QString("S%1-B%2-C%3 %4V").arg(s->groupID()).arg(b->deviceID()).arg(i+1).arg((double)b->cellVoltage(i)/1000));
                        }
                        //evt_log(msg);
                    }
                }
                b->cvAlarmOVHandled(mask);
            }

            if((res = b->cvWarningUVState(&set,&clr)) != 0x00){ // unver voltage warning
                quint16 mask = b->cvWarningUVMask();
                for(int i=0;i<b->cellCount();i++){
                    cmp = (1 << i);
                    if((res & cmp)){
                        if((set & cmp)){
                            mask |= cmp;
                            evt_log("電芯欠壓","一級",s->state(),QString("S%1-B%2-C%3 %4V").arg(s->groupID()).arg(b->deviceID()).arg(i+1).arg((double)b->cellVoltage(i)/1000));
                        }
                        else if(clr & cmp){
                            mask &= ~cmp;
                            evt_log("電芯欠壓復歸","一級",s->state(),QString("S%1-B%2-C%3 %4V").arg(s->groupID()).arg(b->deviceID()).arg(i+1).arg((double)b->cellVoltage(i)/1000));
                        }
                    }
                }
                b->cvWarningUVHandled(mask);
            }

            if((res = b->cvAlarmUVState(&set,&clr)) != 0x00){ // under voltage alarm
                quint16 mask = b->cvAlarmUVMask();
                for(int i=0;i<b->cellCount();i++){
                    cmp = (1 << i);
                    if((res & cmp)){
                        if((set & cmp)){
                            mask |= cmp;
                            evt_log("電芯欠壓","二級",s->state(),QString("S%1-B%2-C%3 %4V").arg(s->groupID()).arg(b->deviceID()).arg(i+1).arg((double)b->cellVoltage(i)/1000));
                        }
                        else if(clr & cmp){
                            mask &= ~cmp;
                            evt_log("電芯欠壓復歸","二級",s->state(),QString("S%1-B%2-C%3 %4V").arg(s->groupID()).arg(b->deviceID()).arg(i+1).arg((double)b->cellVoltage(i)/1000));
                        }
                    }
                }
                b->cvAlarmUVHandled(mask);
            }

            if((res = b->ctWarningOVState(&set,&clr)) != 0x00){ // pack ot warning
                quint16 mask = b->ctWarningOVMask();
                for(int i=0;i<b->ntcCount();i++){
                    cmp = (1 << i);
                    if((res & cmp)){
                        if((set & cmp)){
                            mask |= cmp;
                            evt_log("電池過溫","一級",s->state(),QString("S%1-B%2-C%3 %4\u00b0C").arg(s->groupID()).arg(b->deviceID()).arg(i+1).arg((double)b->packTemperature(i)/10));
                        }
                        else if(clr & cmp){
                            mask &= ~cmp;
                            evt_log("電池過溫復歸","一級",s->state(),QString("S%1-B%2-C%3 %4\u00b0C").arg(s->groupID()).arg(b->deviceID()).arg(i+1).arg((double)b->packTemperature(i)/10));
                        }
                    }
                }
                b->ctWarningOVHandled(mask);
            }

            if((res = b->ctAlarmOVState(&set,&clr)) != 0x00){ // pack ot alarm
                quint16 mask = b->ctAlarmOVMask();
                for(int i=0;i<b->ntcCount();i++){
                    cmp = (1 << i);
                    if((res & cmp)){
                        if((set & cmp)){
                            mask |= cmp;
                            evt_log("電池過溫","二級",s->state(),QString("S%1-B%2-C%3 %4\u00b0C").arg(s->groupID()).arg(b->deviceID()).arg(i+1).arg((double)b->packTemperature(i)/10));
                        }
                        else if(clr & cmp){
                            mask &= ~cmp;
                            evt_log("電池過溫復歸","二級",s->state(),QString("S%1-B%2-C%3 %4\u00b0C").arg(s->groupID()).arg(b->deviceID()).arg(i+1).arg((double)b->packTemperature(i)/10));
                        }
                    }
                }
                b->ctAlarmOVHandled(mask);
            }

            if((res = b->ctWarningUVState(&set,&clr)) != 0x00){ // pack ut warning
                quint16 mask = b->ctWarningUVMask();
                for(int i=0;i<b->ntcCount();i++){
                    cmp = (1 << i);
                    if((res & cmp)){
                        if((set & cmp)){
                            mask |= cmp;
                            evt_log("電池低溫","一級",s->state(),QString("S%1-B%2-C%3 %4\u00b0C").arg(s->groupID()).arg(b->deviceID()).arg(i+1).arg((double)b->packTemperature(i)/10));
                        }
                        else if(clr & cmp){
                            mask &= ~cmp;
                            evt_log("電池低溫復歸","一級",s->state(),QString("S%1-B%2-C%3 %4\u00b0C").arg(s->groupID()).arg(b->deviceID()).arg(i+1).arg((double)b->packTemperature(i)/10));
                        }
                    }
                }
                b->ctWarningUVHandled(mask);
            }

            if((res = b->ctAlarmUVState(&set,&clr)) != 0x00){ // pack ut alarm
                quint16 mask = b->ctAlarmUVMask();
                for(int i=0;i<b->ntcCount();i++){
                    cmp = (1 << i);
                    if((res & cmp)){
                        if((set & cmp)){
                            mask |= cmp;
                            evt_log("電池低溫","二級",s->state(),QString("S%1-B%2-C%3 %4\u00b0C").arg(s->groupID()).arg(b->deviceID()).arg(i+1).arg((double)b->packTemperature(i)/10));
                        }
                        else if(clr & cmp){
                            mask &= ~cmp;
                            evt_log("電池過溫復歸","二級",s->state(),QString("S%1-B%2-C%3 %4\u00b0C").arg(s->groupID()).arg(b->deviceID()).arg(i+1).arg((double)b->packTemperature(i)/10));
                        }
                    }
                }
                b->ctAlarmUVHandled(mask);
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

    rec_log_csv();

    m_socSaveDelay--;
    if(m_socSaveDelay == 0){
        m_socSaveDelay = 300;
        saveCurrentSOC();
        checkDiskSpace();
    }

}

void BMS_System::saveCurrentSOC()
{
    // load soc if exist
    QString socPath;
    if(QSysInfo::productType().contains("win")){
        socPath = "d:/temp/bms/soc";
    }
    else{
        socPath = "/opt/bms/soc";
    }

    QFile f(socPath);
    if(f.open(QIODevice::WriteOnly)){
        QTextStream ts(&f);
        foreach(BMS_Stack *s, m_stacks){
            ts << QString("%1\n").arg(s->sviDevice()->soc());
        }
        f.close();
    }
}

void BMS_System::checkDiskSpace()
{
    if(!m_isController) return;
    // try to move data to sd card if mounted
    QStorageInfo sd_info = QStorageInfo("/mnt/t");

    int sd_size = sd_info.bytesTotal()/1024/1024;
    qDebug()<<"SD Card Installed, Size="<<sd_size;
    QList<QString> activeFiles;
    for(int i=0;i<this->stacks().count();i++){
        QString path = QString("S_0%1_%2.csv").arg(i+1).arg(QDateTime::currentDateTime().toString("yyyyMMdd_hh"));
        activeFiles.append(path);
    }
    if(sd_size > 100){
        int sd_size_free =  sd_info.bytesAvailable()/1024/1024;
        QDir src(this->m_logPath);
        QProcess proc;
        QDir dest("/mnt/t/log");
        if(!dest.exists()){
            QDir().mkpath("/mnt/t/log");
        }
        uint nofFiles = src.count();
        if(nofFiles > 0){
            QString cmd;
            QFileInfoList files = src.entryInfoList(QStringList()<<"*.csv",QDir::Files | QDir::NoDotAndDotDot, QDir::Time | QDir::Reversed);

            for(int i=0;i<files.size();i++){
                const QFileInfo& fi = files[i];
                //if(fi.completeSuffix() == "csv")
                bool move = true;
                foreach (QString s, activeFiles) {
                    if(s == fi.fileName()){
                        move = false;
                        break;
                    }
                }
                if(move){
                    cmd = QString("mv %1 /mnt/t/log/%2").arg(fi.absoluteFilePath()).arg(fi.fileName());
                    proc.execute(cmd);
                    proc.waitForFinished();
                }

//                cmd = QString("rm %1").arg(fi.absoluteFilePath());
//                proc.execute(cmd);
//                proc.waitForFinished();
            }
        }
    }
    else{
        qDebug()<<"NO SD card installed";
        QStorageInfo info=QStorageInfo::root();
        int mb = info.bytesAvailable()/1024/1024;
        if(mb < 100){
            QDir dir(this->m_logPath);
            uint nofFiles = dir.count();
            if(nofFiles > 24){ // kill 24 files (per day)
                QFileInfoList files = dir.entryInfoList(QDir::Files | QDir::NoDotAndDotDot, QDir::Time | QDir::Reversed);
                for(int i=0;i<24;i++){
                    const QFileInfo& info = files.at(i);
                    QFile::remove(info.absoluteFilePath());
                }

            }
        }
    }


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
    int nofStack = sys->m_stacks.size();
    out << nofStack;
    out << sys->m_bcuDevice;
    foreach (BMS_Stack *s, sys->m_stacks) {
        out << s;
    }
    out << sys->m_currentBalanceVoltage;
    out << sys->m_logPath;
    return out;
}

QDataStream& operator >> (QDataStream &in, BMS_System *sys)
{
    int nofStack;
//    in >> sys->Stacks;
    in >> nofStack;
    if(sys->m_bcuDevice == nullptr){
        sys->m_bcuDevice = new BMS_BCUDevice();
    }
    in >> sys->m_bcuDevice;
    if(sys->m_stacks.size() == 0){
        for(quint8 i=0;i<nofStack;i++){
            sys->m_stacks.append(new BMS_Stack());
        }
    }
    ushort minSID=0,minCV = 0xffff;
    for(int i=0;i<sys->m_stacks.size();i++){
        BMS_Stack *s = sys->m_stacks.at(i);
        in >> s;
        if(s->minStackCV() < minCV){
            minCV = s->minStackCV();
            minSID = i;
        }
    }
    sys->m_minVSID = minSID;
    sys->m_minVBID = sys->m_stacks[sys->m_minVSID]->minCVBID();
    sys->m_minVCID = sys->m_stacks[sys->m_minVSID]->minCVCID();
    sys->m_cellMinVoltage = minCV;
    in >> sys->m_currentBalanceVoltage;
    in >> sys->m_logPath;
    return in;
}
//CAN_Packet* BMS_System::setDigitalOut(int ch, int value){
//    if(m_bcuDevice != nullptr){
//        return m_bcuDevice->setDigitalOut(ch,value);
//    }
//    return nullptr;
//}
//CAN_Packet* BMS_System::setVoltageSource(int ch, int value, bool enable){
//    if(m_bcuDevice != nullptr){
//        return m_bcuDevice->setVoltageSource(ch,value,enable);
//    }
//}

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

void BMS_System::ms_poll_100()
{
    if(m_eventRecordCounter > 0){
        m_eventRecordCounter--;
    }
    else{
        m_eventRecordCounter = m_localConfig->record.EventRecordInterval.toInt()*10;
        if(m_eventRecordCounter < 50){
            m_eventRecordCounter = 50;
        }
        eventRecord();
    }

}

void BMS_System::eventRecord()
{
    QString dt = QDateTime::currentDateTime().toString("yyyy/MM/dd hh:mm:ss");
//    for(int i=0;i<m_stacks.size();i++){
//        BMS_Stack *s = m_stacks.at(i);
//        int nofBat = s->BatteryCount();

        QString path = QString("%1/record/eventLog.csv").arg(this->m_logPath);
        QFile f(path);

        if(this->m_eventRecordSize == 0xffff){
            if(f.open(QIODevice::ReadOnly)){
                QTextStream ts(&f);
                ts.setCodec(QTextCodec::codecForName("Big5"));
                quint16 n = 0;
                while(!ts.atEnd()){
                    ts.readLine();
                    n++;
                }
                this->m_eventRecordSize = n;
            }
            f.close();
        }

        if(f.open(QIODevice::ReadWrite | QIODevice::Append)){
            QTextStream ds(&f);
            ds.setCodec(QTextCodec::codecForName("Big5"));
            if(f.size() == 0){ // put header first
                ds <<"Date Time";
                for(int j=0;j<m_stacks.size();j++){
                    ds << QString(",SV:%1,SA:%1,SOC:%1,MODE:%1").arg(j+1);
    //                ds << ",T_MAX("<<QChar(0xb0)<<"C),T_MAX_ID,T_MIN("<<QChar(0xb0)<<"C),T_MIN_ID,DIFFT";
                }
                ds <<"\n";
            }
            else
            {
                ds << dt;
                foreach(BMS_Stack *s, m_stacks){
                    ds << QString(",%1,%2,%3,%4").arg(s->sviDevice()->voltage()/10,5,'f',1).arg(s->sviDevice()->current()/10.,5,'f',1).arg(s->sviDevice()->soc()).arg(s->state());
//                    ds << QString(",%1,%2:%3,%4,%7:%5,%6").arg(s->maxStackCV()).arg(s->maxCVBID()+1).arg(s->maxCVCID()+1).arg(s->minStackCV()).arg(s->minCVCID()+1).arg(s->maxStackCV() - s->minStackCV()).arg(s->minCVBID()+1);
//                    ds << QString(",%1,%2:%3,%4,%7:%5,%6").arg(s->maxStackPT()/10.).arg(s->maxCTBID()+1).arg(s->maxCTTID()+1).arg(s->minStackPT()/10.).arg(s->minCTTID()+1).arg((s->maxStackPT() - s->minStackPT())/10.).arg(s->minCTBID()+1);
                }
                ds <<"\n";
            }
        }
        f.close();
        this->m_eventRecordSize++;

        if(this->m_eventRecordSize > this->m_maxEventRecords){
            // remove records
            QString tmpName = QString("%1/record/tmp.csv").arg(this->m_logPath);

            QFile fin(path);
            QFile fout(tmpName);
            if(fin.open(QIODevice::ReadOnly) && fout.open(QIODevice::ReadWrite)){
                QTextStream ts1(&fin);
                QTextStream ts2(&fout);
                ts2 << ts1.readLine(); // first header
                for(int i=0;i<50;i++){
                    ts1.readLine();
                }

                ts2 << ts1.readAll();
                fin.close();
                fout.close();
                fin.remove();
                fout.rename(path);
            }
            this->m_eventRecordSize -= 50;
        }
//    }
}

void BMS_System::rec_log_csv()
{
    if(!m_enableLog) return;

    QString dt = QDateTime::currentDateTime().toString("yyyy/MM/dd,hh:mm:ss");
    for(int i=0;i<m_stacks.size();i++){
        BMS_Stack *s = m_stacks.at(i);
        int nofBat = s->BatteryCount();
        int g1 = s->batteries().at(0)->cellCount();
        int g2 = s->batteries().at(0)->ntcCount();

        QString path = QString("%1/S_0%2_%3.csv").arg(this->m_logPath).arg(i+1).arg(QDateTime::currentDateTime().toString("yyyyMMdd_hh"));
        QFile f(path);
        if(f.open(QIODevice::ReadWrite | QIODevice::Append)){
            QTextStream ds(&f);
            ds.setCodec(QTextCodec::codecForName("Big5"));
            if(f.size() == 0){ // put header first
                ds <<"Date,Time";
                for(int j=0;j<nofBat;j++){
                    for(int k=0;k<g1;k++){
                        ds << QString(",BMU%1V%2(V)").arg(j+1,2,10,QLatin1Char('0')).arg(k+1,2,10,QLatin1Char('0'));
                    }
                    for(int k=0;k<g2;k++){
                        ds << QString(",BMU%1T%2(%3C)").arg(j+1,2,10,QLatin1Char('0')).arg(k+1,2,10,QLatin1Char('0')).arg(QChar(0xb0));
                    }
                }
                ds << ",SVI-V(V),SVI-C(A),SOC,MODE ";
                ds << ",C_MAX(V),C_MAX_ID,C_MIN,C_MIN_ID,DIFFV";
                ds << ",T_MAX("<<QChar(0xb0)<<"C),T_MAX_ID,T_MIN("<<QChar(0xb0)<<"C),T_MIN_ID,DIFFT";
                ds <<"\n";
            }
            {
                ds << dt;
                for(int j=0;j<nofBat;j++){
                    for(int k=0;k<g1;k++){
                        ds << QString(",%1").arg(s->queueData(j,k)/1000.,5,'f',3);
                    }
                    for(int k=0;k<g2;k++){
                        ds << QString(",%1").arg(s->queueData(j,k+g1+1)/10.,5,'f',1);
                    }
                }
                ds << QString(",%1,%2,%3,%4").arg(s->sviDevice()->voltage()/10,5,'f',1).arg(s->sviDevice()->current()/10.,5,'f',1).arg(s->sviDevice()->soc()).arg(s->state());
                ds << QString(",%1,%2:%3,%4,%7:%5,%6").arg(s->maxStackCV()).arg(s->maxCVBID()+1).arg(s->maxCVCID()+1).arg(s->minStackCV()).arg(s->minCVCID()+1).arg(s->maxStackCV() - s->minStackCV()).arg(s->minCVBID()+1);
                ds << QString(",%1,%2:%3,%4,%7:%5,%6").arg(s->maxStackPT()/10.).arg(s->maxCTBID()+1).arg(s->maxCTTID()+1).arg(s->minStackPT()/10.).arg(s->minCTTID()+1).arg((s->maxStackPT() - s->minStackPT())/10.).arg(s->minCTBID()+1);
                ds <<"\n";
            }
        }
        f.close();
    }
}

void BMS_System::rec_log(QByteArray data){
    if(!m_enableLog) return;
    //QString path = QString("%1/%2.bin").arg(m_logPath).arg(QDateTime::currentMSecsSinceEpoch(),16,16,QLatin1Char('0'));
    QString path = this->m_logPath + "/" + QDateTime::currentDateTime().toString("yyyyMMdd_hhmmss.bin");
    QFile f(path);
    if(f.open(QIODevice::WriteOnly)){
        QDataStream ds(&f);
        ds << data;
        //qDebug()<<"Write rec file:"<<f.size() << " bytes";
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

void BMS_System::evt_log(QString evName, QString evLevel, QString State,QString evInfo)
{
    BMS_Event *evt = new BMS_Event;
    evt->DateString = QDateTime::currentDateTime().toString("yyyy/MM/dd");
    evt->TimeString = QDateTime::currentDateTime().toString("hh:mm:ss");
    evt->EventName = evName;
    evt->Level = evLevel;
    evt->State = State;
    evt->Information = evInfo;

    QString path = this->m_logPath + "/sys";
    if(!QDir(path).exists()){
        QDir().mkpath(path);
    }
    QString fname = path+ "/events.log";
    QFile f(fname);

    if(this->m_eventLogSize == 0xffff){ // first time, count logs
        if(f.open(QIODevice::ReadOnly)){
            QTextStream ts(&f);
            ts.setCodec(QTextCodec::codecForName("Big5"));
            quint16 n = 0;
            //ds.reset();
            while(!ts.atEnd()){
                ts.readLine();
                n++;
            }
            this->m_eventLogSize = n;
        }
        f.close();
    }


    if(f.open(QIODevice::ReadWrite | QIODevice::Append)){
        QTextStream ds(&f);
        ds.setCodec(QTextCodec::codecForName("Big5"));
//        if(this->m_eventLogSize == 0xffff){ // first time, count logs
//            quint16 n = 0;
//            //ds.reset();
//            while(!ds.atEnd()){
//                ds.readLine();
//                n++;
//            }
//            this->m_eventLogSize = n;
//        }
        ds << evt;

        f.close();
        this->m_eventLogSize++;
        if(this->m_eventLogSize > this->m_maxEvents){
            // remove records
            QString tmpName = path + "/tmp.log";
            QFile fin(fname);
            QFile fout(tmpName);
            if(fin.open(QIODevice::ReadOnly) && fout.open(QIODevice::ReadWrite)){
                QTextStream ts1(&fin);
                QTextStream ts2(&fout);
                ts2 << ts1.readLine(); // header
                for(int i=0;i<50;i++){
                    ts1.readLine();
                }

                ts2 << ts1.readAll();
                fin.close();
                fout.close();
                fin.remove();
                fout.rename(fname);
            }
            this->m_eventLogSize -= 50;
        }
    }
}

void BMS_System::sys_log(QString msg)
{
    QString path = this->m_logPath + "/sys";
    if(!QDir(path).exists()){
        QDir().mkpath(path);
    }
    path += "/" + QDateTime::currentDateTime().toString("yyyyMMdd.log");
    QFile f(path);
    if(f.open(QIODevice::WriteOnly)){
        QTextStream ts(&f);
        ts << msg;
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
        this->sys_log(str);
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
    //if(m_cellMinVoltage < BalancingVoltage) return nullptr; // remove 2021-12-01 to avoid not balancing when there is a cell < bv

    bool canBalance = false;
    foreach (BMS_Stack *s, m_stacks) {
        //qDebug()<<"Stacl current:"<<s->sviDevice()->current();
        if(s->sviDevice()->current()> m_balancingCurrentThes){ // +:charge, -:discharge
//        if(s->sviDevice()->current()> -5){ // discharge < 0.5A enable balancing
            canBalance = true;
        }
    }

    CAN_Packet *ret = new CAN_Packet;
    ushort bv = (m_cellMinVoltage > BalancingVoltage)?m_cellMinVoltage:(ushort)BalancingVoltage;
    ret->Command = 0x080;
    ret->remote = false;
    QDataStream ds(&ret->data,QIODevice::WriteOnly);
    ds.setByteOrder(QDataStream::LittleEndian);
    quint8 b = canBalance?0x01:0x0;
//    quint8 b = 0x00;
    ds << b; // enable balancing
    b = BalancingHystersis;
    ds << b;
    ds << bv; // modified 2021-12-01
//    ds << m_cellMinVoltage;
    m_currentBalanceVoltage = m_cellMinVoltage;

    //qDebug()<<"Can balance:"<<canBalance;
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
    return ret;
}

BMS_LocalConfig *BMS_System::config()
{
    return m_localConfig;
}

void BMS_System::setController(bool value)
{
    m_isController = value;
}
