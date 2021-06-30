#include "frmhardwareconfig.h"
#include "ui_frmhardwareconfig.h"
#include "bmscollector.h"
#include "bms_def.h"
#include "bms_bmudevice.h"
#include "bms_bcudevice.h"
#include "bms_svidevice.h"
#include "bms_stack.h"
#include "bms_system.h"
#include "inputwin.h"
#include <QProcess>
#include "bms_localconfig.h"


frmHardwareConfig::frmHardwareConfig(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::frmHardwareConfig)
{
    ui->setupUi(this);
    connect(ui->leVSourceSet_0,&FocusEditor::focused,this,&frmHardwareConfig::on_lineedit_focused);
    connect(ui->leVSourceSet_1,&FocusEditor::focused,this,&frmHardwareConfig::on_lineedit_focused);
    connect(ui->leADCCfgValue,&FocusEditor::focused,this,&frmHardwareConfig::on_lineedit_focused);
    connect(ui->leRTUID,&FocusEditor::focused,this,&frmHardwareConfig::on_lineedit_focused);
    connect(ui->leTCPPort,&FocusEditor::focused,this,&frmHardwareConfig::on_lineedit_focused);

    ui->dateTimeEdit->setDateTime(QDateTime::currentDateTime());
    connect(ui->leLogRecordCounts,&FocusEditor::focused,this,&frmHardwareConfig::on_lineedit_focused);
    connect(ui->leCapacity,&FocusEditor::focused,this,&frmHardwareConfig::on_lineedit_focused);
    connect(ui->leMinBalancingMv,&FocusEditor::focused,this,&frmHardwareConfig::on_lineedit_focused);
    connect(ui->leBalancingTimeSec,&FocusEditor::focused,this,&frmHardwareConfig::on_lineedit_focused);
    connect(ui->leNofStacks,&FocusEditor::focused,this,&frmHardwareConfig::on_lineedit_focused);
    connect(ui->leNofBatteries,&FocusEditor::focused,this,&frmHardwareConfig::on_lineedit_focused);
    connect(ui->leNofCells,&FocusEditor::focused,this,&frmHardwareConfig::on_lineedit_focused);
    connect(ui->leNofNtcs,&FocusEditor::focused,this,&frmHardwareConfig::on_lineedit_focused);
    connect(ui->leHighSet,&FocusEditor::focused,this,&frmHardwareConfig::on_lineedit_focused);
    connect(ui->leHighClr,&FocusEditor::focused,this,&frmHardwareConfig::on_lineedit_focused);
    connect(ui->leLowSet,&FocusEditor::focused,this,&frmHardwareConfig::on_lineedit_focused);
    connect(ui->leLowClr,&FocusEditor::focused,this,&frmHardwareConfig::on_lineedit_focused);
    connect(ui->leDuration,&FocusEditor::focused,this,&frmHardwareConfig::on_lineedit_focused);



    connect(ui->leHighSet,&FocusEditor::textChanged,this,&frmHardwareConfig::on_lineedit_edited);
    connect(ui->leHighClr,&FocusEditor::textChanged,this,&frmHardwareConfig::on_lineedit_edited);
    connect(ui->leLowSet,&FocusEditor::textChanged,this,&frmHardwareConfig::on_lineedit_edited);
    connect(ui->leLowClr,&FocusEditor::textChanged,this,&frmHardwareConfig::on_lineedit_edited);
    connect(ui->leDuration,&FocusEditor::textChanged,this,&frmHardwareConfig::on_lineedit_edited);
}

frmHardwareConfig::~frmHardwareConfig()
{
    delete ui;
}

void frmHardwareConfig::setCollector(BMSCollector *c)
{
    qDebug()<<Q_FUNC_INFO;
    m_collector = c;
    connect(m_collector,&BMSCollector::configReady,this,&frmHardwareConfig::on_system_config_ready);
    connect(m_collector,&BMSCollector::dataReceived,this,&frmHardwareConfig::on_system_data_ready);
}

void frmHardwareConfig::update_collector()
{
    QByteArray dig_in = m_collector->currentSystem()->system->digitalInput();
    QByteArray dig_out = m_collector->currentSystem()->system->digitalOutput();
    QList<int> vs = m_collector->currentSystem()->system->vsource();

    // update UI
    ui->lb_DI0->setText((dig_in[0] & 0x01)==0x01?"SET":"CLEAR");
    ui->lb_DI1->setText((dig_in[0] & 0x02)==0x02?"SET":"CLEAR");

    ui->leVSourceIn_0->setText(QString::number(vs[0]));
    ui->leVSourceIn_1->setText(QString::number(vs[1]));
}

void frmHardwareConfig::on_system_config_ready()
{
}

void frmHardwareConfig::on_system_data_ready()
{
    update_collector();
}

void frmHardwareConfig::on_pbListenSerialPort_clicked()
{
    QPushButton *btn = (QPushButton*)sender();
    if(btn->isChecked()){
        btn->setText("Close");
        ui->cbSerialPort->setEnabled(false);
        int b = ui->cbBaudrate->currentText().toInt();
        m_collector->currentSystem()->openSerialPort(ui->cbSerialPort->currentText(),b,1,1);
    }
    else{
        m_collector->currentSystem()->closeSerialPort(ui->cbSerialPort->currentText());
        ui->cbSerialPort->setEnabled(true);
    }
}

void frmHardwareConfig::on_pbWriteSerialPort_clicked()
{
    if(m_collector == nullptr) return;
    m_collector->currentSystem()->writeSerialPort(ui->cbSerialPort->currentText(),ui->leSeriaalWrite->text());
}

void frmHardwareConfig::on_pbRelay_0_clicked()
{
    if(m_collector == nullptr) return;
    if(ui->pbRelay_0->isChecked()){
        m_collector->currentSystem()->setDigitalOut(0,1);
    }
    else{
        m_collector->currentSystem()->setDigitalOut(0,0);
    }
}

void frmHardwareConfig::on_pbRelay_1_clicked()
{
    if(m_collector == nullptr) return;
    if(m_collector->currentSystem() == nullptr) return;
    if(ui->pbRelay_1->isChecked()){
        m_collector->currentSystem()->setDigitalOut(1,1);
    }
    else{
        m_collector->currentSystem()->setDigitalOut(1,0);
    }
}

void frmHardwareConfig::on_pbADCConfig_clicked()
{
    if(m_collector == nullptr) return;
    if(m_collector->currentSystem() == nullptr) return;
    QString command;
    command = QString("BCU:AIMAP:%1:%2:%3").arg(ui->cbADCChannel->currentIndex()).arg(ui->cbADCOption->currentIndex()).arg(ui->leADCCfgValue->text());
    m_collector->currentSystem()->writeCommand(command);
}

void frmHardwareConfig::on_pushButton_9_clicked()
{
    if(m_collector == nullptr) return;
    if(m_collector->currentSystem() == nullptr) return;
    int value = ui->leVSourceSet_0->text().toInt();
    m_collector->currentSystem()->setVoltageSource(0,value);

}

void frmHardwareConfig::on_pushButton_10_clicked()
{
    if(m_collector == nullptr) return;
    if(m_collector->currentSystem() == nullptr) return;
    int value = ui->leVSourceSet_1->text().toInt();
    m_collector->currentSystem()->setVoltageSource(1,value);
}

void frmHardwareConfig::on_pbSaveParam_clicked()
{
    if(m_collector == nullptr) return;
    if(m_collector->currentSystem() == nullptr) return;
    m_collector->currentSystem()->writeCommand("BCU:SAVE");
}

void frmHardwareConfig::on_pbEnVS0_clicked()
{
    if(m_collector == nullptr) return;
    if(m_collector->currentSystem() == nullptr) return;
    m_collector->currentSystem()->setVoltageSource(0,0);

}

void frmHardwareConfig::on_pbEnVS1_clicked()
{
    if(m_collector == nullptr) return;
    if(m_collector->currentSystem() == nullptr) return;
    m_collector->currentSystem()->setVoltageSource(1,0);

}

void frmHardwareConfig::on_lineedit_focused(bool state)
{
    qDebug()<<Q_FUNC_INFO;
    FocusEditor *editor = (FocusEditor*)sender();
    InputWin *w = new InputWin;
    w->setAttribute(Qt::WA_DeleteOnClose);
    w->setDisplayContent(editor->text());
    connect(w,&InputWin::result,editor,&FocusEditor::setText);
    w->exec();
}


void frmHardwareConfig::on_pushButton_9_clicked(bool checked)
{

}

void frmHardwareConfig::on_pbSVIConfig_clicked()
{
    if(m_collector == nullptr) return;
    if(m_collector->currentSystem() == nullptr) return;
    QString command;
    command = QString("SVI:AIMAP:%1:%2:%3").arg(ui->cbSVIChannel->currentIndex()).arg(ui->cbSVIOption->currentIndex()).arg(ui->leSVIValue->text());
    m_collector->currentSystem()->writeCommand(command);
}

void frmHardwareConfig::on_pbSaveSVI_clicked()
{
    if(m_collector == nullptr) return;
    if(m_collector->currentSystem() == nullptr) return;
    m_collector->currentSystem()->writeCommand("SVI:SAVE");
}

void frmHardwareConfig::on_pushButton_clicked()
{
    if(m_collector == nullptr) return;
    if(m_collector->currentSystem() == nullptr) return;
    QString command;
    command = QString("BMU:BV:%1:%2:%3:%4:%5").arg(ui->leBMUBV->text()).arg(ui->leBMUHV->text()).arg(ui->cbBalancing->isChecked()?"1":"0").arg(ui->leBMUBON->text()).arg(ui->leBMUBOFF->text());
    m_collector->currentSystem()->writeCommand(command);
}

void frmHardwareConfig::on_pbBMUBE_clicked()
{
    if(m_collector == nullptr) return;
    if(m_collector->currentSystem() == nullptr) return;
    QString command;
    command = QString("BMU:BE:1");
    m_collector->currentSystem()->writeCommand(command);
}

void frmHardwareConfig::on_pbBMUNBE_clicked()
{
    if(m_collector == nullptr) return;
    if(m_collector->currentSystem() == nullptr) return;
    QString command;
    command = QString("BMU:BE:0");
    m_collector->currentSystem()->writeCommand(command);
}

void frmHardwareConfig::on_pbRestartController_clicked()
{
    QProcess *proc = new QProcess;

    proc->start("pkill BMS_Controller");
    proc->waitForFinished();

    delete proc;
}

void frmHardwareConfig::on_pbDisableController_clicked()
{
    QProcess *proc = new QProcess;

//    proc->start("sh",QStringList()<<" -c"<< "systemctl stop bms_controller");
    proc->start("systemctl stop bms_controller");
    proc->waitForFinished();
    qDebug()<<"Stop controller "<<QString(proc->readAll());
//    proc->start("sh",QStringList()<<" -c"<< "systemctl disable bms_controller");
    proc->start("systemctl disable bms_controller");
    proc->waitForFinished();
    qDebug()<<"Stop controller "<<QString(proc->readAll());

    delete proc;
}

void frmHardwareConfig::on_pbEnableController_clicked()
{
    QProcess *proc = new QProcess;

    proc->start("systemctl enable bms_controller");
    proc->waitForFinished();

    proc->start("systemctl start bms_controller");
    proc->waitForFinished();

    delete proc;

}

void frmHardwareConfig::on_pbSetDateTime_clicked()
{
    QString dts ;
    QDateTime dt = ui->dateTimeEdit->dateTime();
    dts = dt.toString("timedatectl set-time \'yyyy-MM-dd hh:mm:00\'");
    qDebug()<<"Set Time "<<dts;

    QProcess *proc = new QProcess();
    proc->start(dts);
    proc->waitForFinished();
    delete proc;
}

void frmHardwareConfig::on_pbAlarmToSD_clicked()
{
    if(QSysInfo::productType().contains("win")) return;


}

void frmHardwareConfig::on_pbRecordToSD_clicked()
{

}

void frmHardwareConfig::setLocalConfig(QString cfgName)
{

}


void frmHardwareConfig::on_pbLoadLocalSetting_clicked()
{
    // load from file
    QString path;

    if(QSysInfo::productType().contains("win")){
        path = "./config/local2.json";
    }
    else{
       path = QCoreApplication::applicationDirPath()+"/config/local.json";
    }
    localConfig.load(path);
    updateLocalSetting();

}

void frmHardwareConfig::on_pbCellWarning_clicked()
{
    CurrentCriteria = "cell-volt-warning";

    ui->lb_hset->setText(tr("高限作動(V)"));
    ui->lb_hclr->setText(tr("高限復歸(V)"));
    ui->lb_lset->setText(tr("低限作動(V)"));
    ui->lb_lclr->setText(tr("低限復歸(V)"));

    ui->leHighSet->setText((localConfig.criteria.cell.volt_warning.High_Set));
    ui->leHighClr->setText((localConfig.criteria.cell.volt_warning.High_Clr));
    ui->leLowSet->setText((localConfig.criteria.cell.volt_warning.Low_Set));
    ui->leLowClr->setText((localConfig.criteria.cell.volt_warning.Low_Clr));
    ui->leDuration->setText((localConfig.criteria.cell.volt_warning.Duration));
    ui->leHighSet->setEnabled(true);
    ui->leHighClr->setEnabled(true);
}

void frmHardwareConfig::on_pbCellAlarm_clicked()
{
    CurrentCriteria = "cell-volt-alarm";
    ui->lb_hset->setText(tr("高限作動(V)"));
    ui->lb_hclr->setText(tr("高限復歸(V)"));
    ui->lb_lset->setText(tr("低限作動(V)"));
    ui->lb_lclr->setText(tr("低限復歸(V)"));
    ui->leHighSet->setText((localConfig.criteria.cell.volt_alarm.High_Set));
    ui->leHighClr->setText((localConfig.criteria.cell.volt_alarm.High_Clr));
    ui->leLowSet->setText((localConfig.criteria.cell.volt_alarm.Low_Set));
    ui->leLowClr->setText((localConfig.criteria.cell.volt_alarm.Low_Clr));
    ui->leDuration->setText((localConfig.criteria.cell.volt_alarm.Duration));
    ui->leHighSet->setEnabled(true);
    ui->leHighClr->setEnabled(true);
}

void frmHardwareConfig::on_pbBatteryWarning_clicked()
{
    CurrentCriteria = "cell-temp-warning";
    ui->lb_hset->setText(QString::fromUtf8("高限作動(\u00b0C)"));
    ui->lb_hclr->setText(QString::fromUtf8("高限復歸(\u00b0C)"));
    ui->lb_lset->setText(QString::fromUtf8("低限作動(\u00b0C)"));
    ui->lb_lclr->setText(QString::fromUtf8("低限復歸(\u00b0C)"));
    ui->leHighSet->setText((localConfig.criteria.cell.temp_warning.High_Set));
    ui->leHighClr->setText((localConfig.criteria.cell.temp_warning.High_Clr));
    ui->leLowSet->setText((localConfig.criteria.cell.temp_warning.Low_Set));
    ui->leLowClr->setText((localConfig.criteria.cell.temp_warning.Low_Clr));
    ui->leDuration->setText((localConfig.criteria.cell.temp_warning.Duration));
    ui->leHighSet->setEnabled(true);
    ui->leHighClr->setEnabled(true);
}

void frmHardwareConfig::on_pbBatteryAlarm_clicked()
{
    CurrentCriteria = "cell-temp-alarm";
    ui->lb_hset->setText(QString::fromUtf8("高限作動(\u00b0C)"));
    ui->lb_hclr->setText(QString::fromUtf8("高限復歸(\u00b0C)"));
    ui->lb_lset->setText(QString::fromUtf8("低限作動(\u00b0C)"));
    ui->lb_lclr->setText(QString::fromUtf8("低限復歸(\u00b0C)"));
    ui->leHighSet->setText((localConfig.criteria.cell.temp_alarm.High_Set));
    ui->leHighClr->setText((localConfig.criteria.cell.temp_alarm.High_Clr));
    ui->leLowSet->setText((localConfig.criteria.cell.temp_alarm.Low_Set));
    ui->leLowClr->setText((localConfig.criteria.cell.temp_alarm.Low_Clr));
    ui->leDuration->setText((localConfig.criteria.cell.temp_alarm.Duration));
    ui->leHighSet->setEnabled(true);
    ui->leHighClr->setEnabled(true);
}

void frmHardwareConfig::on_pbStackWarning_clicked()
{
    CurrentCriteria = "stack-volt-warning";
    ui->lb_hset->setText(tr("高限作動(V)"));
    ui->lb_hclr->setText(tr("高限復歸(V)"));
    ui->lb_lset->setText(tr("低限作動(V)"));
    ui->lb_lclr->setText(tr("低限復歸(V)"));
    ui->leHighSet->setText((localConfig.criteria.stack.volt_warning.High_Set));
    ui->leHighClr->setText((localConfig.criteria.stack.volt_warning.High_Clr));
    ui->leLowSet->setText((localConfig.criteria.stack.volt_warning.Low_Set));
    ui->leLowClr->setText((localConfig.criteria.stack.volt_warning.Low_Clr));
    ui->leDuration->setText((localConfig.criteria.stack.volt_warning.Duration));
    ui->leHighSet->setEnabled(true);
    ui->leHighClr->setEnabled(true);
}

void frmHardwareConfig::on_pbStackAlarm_clicked()
{
    CurrentCriteria = "stack-volt-alarm";
    ui->lb_hset->setText(tr("高限作動(V)"));
    ui->lb_hclr->setText(tr("高限復歸(V)"));
    ui->lb_lset->setText(tr("低限作動(V)"));
    ui->lb_lclr->setText(tr("低限復歸(V)"));
    ui->leHighSet->setText((localConfig.criteria.stack.volt_alarm.High_Set));
    ui->leHighClr->setText((localConfig.criteria.stack.volt_alarm.High_Clr));
    ui->leLowSet->setText((localConfig.criteria.stack.volt_alarm.Low_Set));
    ui->leLowClr->setText((localConfig.criteria.stack.volt_alarm.Low_Clr));
    ui->leDuration->setText((localConfig.criteria.stack.volt_alarm.Duration));
    ui->leHighSet->setEnabled(true);
    ui->leHighClr->setEnabled(true);
}
void frmHardwareConfig::on_pbSOCWarning_clicked()
{
    CurrentCriteria = "soc-warning";
    ui->lb_hset->setText(tr(""));
    ui->lb_hclr->setText(tr(""));
    ui->lb_lset->setText(tr("低限作動(%)"));
    ui->lb_lclr->setText(tr("低限復歸(%)"));
//    ui->leHighSet->setText((localConfig.criteria.soc.warning.High_Set));
//    ui->leHighClr->setText((localConfig.criteria.soc.warning.High_Clr));
    ui->leHighSet->setText(("0"));
    ui->leHighClr->setText(("0"));
    ui->leHighSet->setEnabled(false);
    ui->leHighClr->setEnabled(false);
    ui->leLowSet->setText((localConfig.criteria.soc.warning.Low_Set));
    ui->leLowClr->setText((localConfig.criteria.soc.warning.Low_Clr));
    ui->leDuration->setText((localConfig.criteria.soc.warning.Duration));
}

void frmHardwareConfig::on_pbSOCAlarm_clicked()
{
    CurrentCriteria = "soc-alarm";
    ui->lb_hset->setText(tr(""));
    ui->lb_hclr->setText(tr(""));
    ui->lb_lset->setText(tr("低限作動(%)"));
    ui->lb_lclr->setText(tr("低限復歸(%)"));
//    ui->leHighSet->setText((localConfig.criteria.soc.alarm.High_Set));
//    ui->leHighClr->setText((localConfig.criteria.soc.alarm.High_Clr));
    ui->leHighSet->setText(("0"));
    ui->leHighClr->setText(("0"));
    ui->leHighSet->setEnabled(false);
    ui->leHighClr->setEnabled(false);
    ui->leLowSet->setText((localConfig.criteria.soc.alarm.Low_Set));
    ui->leLowClr->setText((localConfig.criteria.soc.alarm.Low_Clr));
    ui->leDuration->setText((localConfig.criteria.soc.alarm.Duration));
}

void frmHardwareConfig::on_lineedit_edited(QString text)
{
    qDebug()<<Q_FUNC_INFO;
    FocusEditor *editor = (FocusEditor*)sender();
    if(CurrentCriteria == "cell-volt-warning"){
        if(editor == ui->leHighSet){
            localConfig.criteria.cell.volt_warning.High_Set = text;
        }
        else if(editor == ui->leHighClr){
            localConfig.criteria.cell.volt_warning.High_Clr = text;
        }
        else if(editor == ui->leLowSet){
            localConfig.criteria.cell.volt_warning.Low_Set = text;
        }
        else if(editor == ui->leLowClr){
            localConfig.criteria.cell.volt_warning.Low_Clr = text;
        }
        else if(editor == ui->leDuration){
            localConfig.criteria.cell.volt_warning.Duration = text;
        }
    }
    else if(CurrentCriteria == "cell-volt-alarm"){
        if(editor == ui->leHighSet){
            localConfig.criteria.cell.volt_alarm.High_Set = text;
        }
        else if(editor == ui->leHighClr){
            localConfig.criteria.cell.volt_alarm.High_Clr = text;
        }
        else if(editor == ui->leLowSet){
            localConfig.criteria.cell.volt_alarm.Low_Set = text;
        }
        else if(editor == ui->leLowClr){
            localConfig.criteria.cell.volt_alarm.Low_Clr = text;
        }
        else if(editor == ui->leDuration){
            localConfig.criteria.cell.volt_alarm.Duration = text;
        }
    }
    else if(CurrentCriteria == "cell-temp-warning"){
        if(editor == ui->leHighSet){
            localConfig.criteria.cell.temp_warning.High_Set = text;
        }
        else if(editor == ui->leHighClr){
            localConfig.criteria.cell.temp_warning.High_Clr = text;
        }
        else if(editor == ui->leLowSet){
            localConfig.criteria.cell.temp_warning.Low_Set = text;
        }
        else if(editor == ui->leLowClr){
            localConfig.criteria.cell.temp_warning.Low_Clr = text;
        }
        else if(editor == ui->leDuration){
            localConfig.criteria.cell.temp_warning.Duration = text;
        }
    }
    else if(CurrentCriteria == "cell-temp-alarm"){
        if(editor == ui->leHighSet){
            localConfig.criteria.cell.temp_alarm.High_Set = text;
        }
        else if(editor == ui->leHighClr){
            localConfig.criteria.cell.temp_alarm.High_Clr = text;
        }
        else if(editor == ui->leLowSet){
            localConfig.criteria.cell.temp_alarm.Low_Set = text;
        }
        else if(editor == ui->leLowClr){
            localConfig.criteria.cell.temp_alarm.Low_Clr = text;
        }
        else if(editor == ui->leDuration){
            localConfig.criteria.cell.temp_alarm.Duration = text;
        }
    }
    else if(CurrentCriteria == "stack-volt-warning"){
        if(editor == ui->leHighSet){
            localConfig.criteria.stack.volt_warning.High_Set = text;
        }
        else if(editor == ui->leHighClr){
            localConfig.criteria.stack.volt_warning.High_Clr = text;
        }
        else if(editor == ui->leLowSet){
            localConfig.criteria.stack.volt_warning.Low_Set = text;
        }
        else if(editor == ui->leLowClr){
            localConfig.criteria.stack.volt_warning.Low_Clr = text;
        }
        else if(editor == ui->leDuration){
            localConfig.criteria.stack.volt_warning.Duration = text;
        }
    }
    else if(CurrentCriteria == "stack-volt-alarm"){
        if(editor == ui->leHighSet){
            localConfig.criteria.stack.volt_alarm.High_Set = text;
        }
        else if(editor == ui->leHighClr){
            localConfig.criteria.stack.volt_alarm.High_Clr = text;
        }
        else if(editor == ui->leLowSet){
            localConfig.criteria.stack.volt_alarm.Low_Set = text;
        }
        else if(editor == ui->leLowClr){
            localConfig.criteria.stack.volt_alarm.Low_Clr = text;
        }
        else if(editor == ui->leDuration){
            localConfig.criteria.stack.volt_alarm.Duration = text;
        }
    }
    else if(CurrentCriteria == "soc-warning"){
        if(editor == ui->leHighSet){
            localConfig.criteria.soc.warning.High_Set = text;
        }
        else if(editor == ui->leHighClr){
            localConfig.criteria.soc.warning.High_Clr = text;
        }
        else if(editor == ui->leLowSet){
            localConfig.criteria.soc.warning.Low_Set = text;
        }
        else if(editor == ui->leLowClr){
            localConfig.criteria.soc.warning.Low_Clr = text;
        }
        else if(editor == ui->leDuration){
            localConfig.criteria.soc.warning.Duration = text;
        }
    }
    else if(CurrentCriteria == "soc-alarm"){
        if(editor == ui->leHighSet){
            localConfig.criteria.soc.alarm.High_Set = text;
        }
        else if(editor == ui->leHighClr){
            localConfig.criteria.soc.alarm.High_Clr = text;
        }
        else if(editor == ui->leLowSet){
            localConfig.criteria.soc.alarm.Low_Set = text;
        }
        else if(editor == ui->leLowClr){
            localConfig.criteria.soc.alarm.Low_Clr = text;
        }
        else if(editor == ui->leDuration){
            localConfig.criteria.soc.alarm.Duration = text;
        }
    }
}

void frmHardwareConfig::on_pbSaveLocalConfig_clicked()
{
    QString path;

    if(QSysInfo::productType().contains("win")){
        path = "./config/local2.json";
    }
    else{
       path = QCoreApplication::applicationDirPath()+"/config/local.json";
    }

    localConfig.EnableLog = ui->cbEnableLog->isChecked();
    if(ui->cbRecordType->currentIndex() == 0){ // by day
        localConfig.LogDays = ui->leLogRecordCounts->text();
        localConfig.LogRecords =  "-1";
    }
    else{ // by record
        localConfig.LogDays = "-1";
        localConfig.LogRecords = ui->leLogRecordCounts->text();
    }

    localConfig.balancing.BalancingVolt = ui->leMinBalancingMv->text();
    localConfig.balancing.On_TimeSec = ui->leBalancingTimeSec->text();
    localConfig.balancing.Off_TimeSec = localConfig.balancing.On_TimeSec;
    localConfig.balancing.HystersisMV = "8";


    localConfig.stack.StackCount = ui->leNofStacks->text();
    localConfig.stack.BatteryPerStack = ui->leNofBatteries->text();
    localConfig.stack.CellPerBattery = ui->leNofCells->text();
    localConfig.stack.NTCPerBattery = ui->leNofNtcs->text();
    localConfig.stack.Capacity = ui->leCapacity->text();

    localConfig.modbus.Enable = ui->cbMBSRTUEnable->isChecked();
    localConfig.modbus.Port = ui->cbRTUPort->currentText().trimmed();
    localConfig.modbus.Bitrate = ui->cbRTUBaudrate->currentText().trimmed();
    localConfig.modbus.Parity = ui->cbRTUParity->currentText().trimmed();
    localConfig.modbus.ID = ui->leRTUID->text();


    localConfig.save(path);
}

void frmHardwareConfig::updateLocalSetting()
{
    ui->cbEnableLog->setChecked(localConfig.EnableLog);
    if(localConfig.LogDays.toInt() == -1){
        ui->cbRecordType->setCurrentIndex(1);
        ui->leLogRecordCounts->setText((localConfig.LogRecords));
    }
    else{
        ui->cbRecordType->setCurrentIndex(1);
        ui->leLogRecordCounts->setText((localConfig.LogDays));
    }

    ui->leNofStacks->setText((localConfig.stack.StackCount));
    ui->leNofBatteries->setText((localConfig.stack.BatteryPerStack));
    ui->leNofCells->setText((localConfig.stack.CellPerBattery));
    ui->leNofNtcs->setText((localConfig.stack.NTCPerBattery));
    ui->leCapacity->setText((localConfig.stack.Capacity));

    CurrentCriteria = "cell-volt-warning";
    ui->leHighSet->setText((localConfig.criteria.cell.volt_warning.High_Set));
    ui->leHighClr->setText((localConfig.criteria.cell.volt_warning.High_Clr));
    ui->leLowSet->setText((localConfig.criteria.cell.volt_warning.Low_Set));
    ui->leLowClr->setText((localConfig.criteria.cell.volt_warning.Low_Clr));
    ui->leDuration->setText((localConfig.criteria.cell.volt_warning.Duration));

    ui->leMinBalancingMv->setText((localConfig.balancing.BalancingVolt));
    ui->leBalancingTimeSec->setText((localConfig.balancing.On_TimeSec));

    ui->cbMBSRTUEnable->setChecked(localConfig.modbus.Enable);
    if(localConfig.modbus.Bitrate == "9600"){
        ui->cbBaudrate->setCurrentIndex(0);
    }
    else if(localConfig.modbus.Bitrate == "19200"){
        ui->cbBaudrate->setCurrentIndex(1);
    }
    else if(localConfig.modbus.Bitrate == "38400"){
        ui->cbBaudrate->setCurrentIndex(2);
    }
    else if(localConfig.modbus.Bitrate == "57600"){
        ui->cbBaudrate->setCurrentIndex(3);
    }
    else if(localConfig.modbus.Bitrate == "115200"){
        ui->cbBaudrate->setCurrentIndex(4);
    }

    if(localConfig.modbus.Parity == "NONE"){
        ui->cbRTUParity->setCurrentIndex(0);
    }
    else if(localConfig.modbus.Parity == "EVEN"){
        ui->cbRTUParity->setCurrentIndex(1);
    }
    else if(localConfig.modbus.Parity == "ODD"){
        ui->cbRTUParity->setCurrentIndex(2);
    }

    ui->leRTUID->setText(localConfig.modbus.ID);
}

void frmHardwareConfig::set_backlight(int brightness, bool off)
{
    QProcess *proc = new QProcess;
    QString cmd;
    if(off){
        cmd = QString("echo 0 > /sys/class/backlight/backlight-lvds/bl_power");
        proc->start(cmd);
        proc->waitForFinished();
    }
    else{
        cmd = QString("echo 1 > /sys/class/backlight/backlight-lvds/bl_power");
        proc->start(cmd);
        proc->waitForFinished();
        cmd = QString("echo %1 > /sys/class/backlight/backlight-lvds/brightness").arg(brightness);
        proc->start(cmd);
        proc->waitForFinished();
    }
    delete proc;
}

