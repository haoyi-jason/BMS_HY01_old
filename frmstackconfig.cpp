#include "frmstackconfig.h"
#include "ui_frmstackconfig.h"
#include "bms_model.h"
#include <QFileDialog>
#include "bms_bmudevice.h"
#include "bms_bcudevice.h"
#include "bms_svidevice.h"
#include "bms_stack.h"
#include "bms_system.h"
#include <QMessageBox>


frmStackConfig::frmStackConfig(QString configPath, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::frmStackConfig)
{
    ui->setupUi(this);
    this->m_configPath = configPath;

    if(!QFileInfo(this->m_configPath).exists()){
        if(QSysInfo::productType().contains("win")){
            this->m_configPath="d:/temp/bms/config/controller.json";
        }
        else{
            this->m_configPath = "/opt/bms/config/controller.json"; //-- change after Jul. 21'
        }
    }
    m_localConfig = new BMS_LocalConfig();
    m_localConfig->load(this->m_configPath);

    connect(ui->pb_next_stack,&QPushButton::clicked,this,&frmStackConfig::on_change_stack_view);
    connect(ui->pb_previous_stack,&QPushButton::clicked,this,&frmStackConfig::on_change_stack_view);

    alarmModel = new BMS_AlarmModel(m_localConfig);
    ui->tv_alarm->setModel(alarmModel);

    connect(ui->tv_alarm, &QTableView::clicked, this,&frmStackConfig::on_tableview_clicked);
}

frmStackConfig::~frmStackConfig()
{
    delete ui;
}

void frmStackConfig::on_pbSetPath_clicked()
{

}

void frmStackConfig::on_pbScan_clicked()
{

}

void frmStackConfig::set_config_to_ui()
{

}

void frmStackConfig::load_config_from_ui()
{

}

void frmStackConfig::on_lwFiles_doubleClicked(const QModelIndex &index)
{
//    int id = index.row();
//    QString fileName = ui->leConfigPath->text() + "\\" + index.data().toString();
//    qDebug()<<"File : "<<fileName <<" selected;";

//    QFile f(fileName);
//    f.open(QIODevice::ReadOnly);

//    BMS_System *sys = new BMS_System();

//    if(!sys->Configuration(f.readAll())){
//        qDebug()<<"Parse Json fail";
//    }

//    f.close();

//    ui->leAlias->setText(sys->Alias);
//    ui->leIpAddress->setText(sys->connectionString);
//    ui->leBalancingActiveTIme->setText(QString::number(sys->BalancingOnTime));
//    ui->leBalancingHystersis->setText(QString::number(sys->BalancingHystersis));
//    ui->leBalancingVoltage->setText(QString::number(sys->BalancingVoltage));
//    ui->leRptNormal->setText(QString::number(sys->normalReportMS));
//    ui->leRptError->setText(QString::number(sys->errorReportMS));
//    ui->leNofStacks->setText(QString::number(sys->Stacks));
//    //ui->leNofBatteries->setText(QString::number(sys->BatteryPerStack));
//    //ui->leNofCells->setText(QString::number(sys->CellPerBattery));
//    //ui->leNofNTC->setText(QString::number(sys->NTCPerBattery));

}

void frmStackConfig::on_change_stack_view()
{
    QPushButton *btn = (QPushButton*)sender();
    int index = ui->stackedWidget->currentIndex();
    int n = ui->stackedWidget->count();
    if(btn == ui->pb_next_stack){
        index++;
        if(index == n) index--;
    }
    else if(btn == ui->pb_previous_stack){
        index--;
        if(index < 0) index = 0;
    }
    ui->stackedWidget->setCurrentIndex(index);
}

void frmStackConfig::set_text_value(QLineEdit *edit, QString value)
{
    edit->setText(value);
}

QString frmStackConfig::get_text_value(QLineEdit *edit)
{
    return edit->text();
}

void frmStackConfig::on_tableview_clicked(const QModelIndex &index)
{
    InputWin w;
    if(index.isValid() && index.column()!=0){
        w.setDisplayContent(index.data().toString());
        if(w.exec() == QDialog::Accepted ){
            QMessageBox::information(this,"INFO","INFO");
        }
    }

}
