#include "frmstackconfig.h"
#include "ui_frmstackconfig.h"
#include "bms_model.h"
#include <QFileDialog>

frmStackConfig::frmStackConfig(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::frmStackConfig)
{
    ui->setupUi(this);
}

frmStackConfig::~frmStackConfig()
{
    delete ui;
}

void frmStackConfig::on_pbSetPath_clicked()
{
    QString path = QFileDialog::getExistingDirectory(this,"Select Path");
    if(!path.isEmpty()){
        ui->leConfigPath->setText(path);
    }
}

void frmStackConfig::on_pbScan_clicked()
{
    // scan file under working directory
    QString path = ui->leConfigPath->text();
    if(path.isNull()){
        return ;
    }

    QStringList f_list = QDir(path).entryList(QStringList()<<"*.json",QDir::Files);
    if(f_list.size() > 0){
        ui->lwFiles->clear();
        ui->lwFiles->addItems(f_list);
    }
}

void frmStackConfig::on_lwFiles_doubleClicked(const QModelIndex &index)
{
    int id = index.row();
    QString fileName = ui->leConfigPath->text() + "\\" + index.data().toString();
    qDebug()<<"File : "<<fileName <<" selected;";

    QFile f(fileName);
    f.open(QIODevice::ReadOnly);

    BMS_SystemInfo *sys = new BMS_SystemInfo();

    if(!sys->Configuration(f.readAll())){
        qDebug()<<"Parse Json fail";
    }

    f.close();

    ui->leAlias->setText(sys->Alias);
    ui->leIpAddress->setText(sys->connectionString);
    ui->leBalancingActiveTIme->setText(QString::number(sys->BalancingOnTime));
    ui->leBalancingHystersis->setText(QString::number(sys->BalancingHystersis));
    ui->leBalancingVoltage->setText(QString::number(sys->BalancingVoltage));
    ui->leRptNormal->setText(QString::number(sys->normalReportMS));
    ui->leRptError->setText(QString::number(sys->errorReportMS));
    ui->leNofStacks->setText(QString::number(sys->Stacks));
    //ui->leNofBatteries->setText(QString::number(sys->BatteryPerStack));
    //ui->leNofCells->setText(QString::number(sys->CellPerBattery));
    //ui->leNofNTC->setText(QString::number(sys->NTCPerBattery));

}


