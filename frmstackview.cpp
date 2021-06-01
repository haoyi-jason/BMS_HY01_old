#include "frmstackview.h"
#include "ui_frmstackview.h"
#include "bms_model.h"
#include "bmscollector.h"

frmStackView::frmStackView(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::frmStackView)
{
    ui->setupUi(this);
    batteryModel = new BMS_BatteryModel();
    stackModel = new BMS_StackModel();
    ui->lbInfo->setText("");
}

frmStackView::~frmStackView()
{
    delete ui;
}

void frmStackView::setCollector(BMSCollector *collector)
{
    this->collector = collector;
    QObject::connect(this->collector,&BMSCollector::configReady,this,&frmStackView::on_system_config_ready);
    QObject::connect(this->collector,&BMSCollector::dataReceived,this,&frmStackView::on_system_data_ready);
//    if(this->collector->currentSystem()->system != nullptr){
//        stackModel->setStack(this->collector->currentSystem()->system->stacks());
//        batteryModel->setStack(stackModel->findStack(0));
//        ui->tableView->setModel(batteryModel);
//    }
}

void frmStackView::on_system_config_ready()
{
    if(collector->currentSystem() != nullptr){
        activeSystem = collector->currentSystem()->system;
    }
    else{
        activeSystem = nullptr;
    }
    if(activeSystem != nullptr){
        stackModel->setStack(activeSystem->stacks());
        batteryModel->setStack(stackModel->findStack(0));
        ui->tableView->setModel(batteryModel);
        m_currentStackIndex = 0;
    }
}

void frmStackView::on_system_data_ready()
{
    if(activeSystem != nullptr){
        ui->tableView->viewport()->update();
        updateStackInfo();
    }
}

void frmStackView::on_pbSwitchInfo_clicked()
{
    int c = ui->stackedWidget->currentIndex();
    c++;
    if(c >= ui->stackedWidget->count()){
        c = 0;
    }
    ui->stackedWidget->setCurrentIndex(c);
}

void frmStackView::on_pbPreviousStack_clicked()
{
    m_currentStackIndex--;
    if(m_currentStackIndex<0){
        m_currentStackIndex = stackModel->rowCount()-1;
    }
    batteryModel->setStack(stackModel->findStack(m_currentStackIndex));
    ui->tableView->viewport()->update();
    updateStackInfo();
}

void frmStackView::on_pbNextStack_clicked()
{
    m_currentStackIndex++;
    if(m_currentStackIndex==stackModel->rowCount()){
        m_currentStackIndex = 0;
    }
    batteryModel->setStack(stackModel->findStack(m_currentStackIndex));
    ui->tableView->viewport()->update();
    updateStackInfo();
}

void frmStackView::updateStackInfo()
{
    QString message;
    message += QString("系統名稱：%1\r\n").arg(collector->currentSystem()->system->alias());
    message += QString("系統總簇數：%1\r\n").arg(collector->currentSystem()->system->Stacks);
    message += QString("目前在第 [ %1 ] 簇").arg(m_currentStackIndex+1);
    ui->lbInfo->setText(message);

    BMS_StackInfo *stack = stackModel->findStack(m_currentStackIndex);
    ui->leMaxCellVoltage->setText(QString::number(stack->maxCellVoltage()));
    ui->le_minVoltage->setText(QString::number(stack->minCellVoltage()));
    ui->le_maxTemp->setText(QString::number(stack->maxStackTemperature()));
    ui->le_minTemp->setText(QString::number(stack->minStackTemperature()));
    ui->leTotalVoltage->setText(QString::number(stack->stackVoltage()));
    ui->le_current->setText(QString::number(stack->stackCurrent()));
}
