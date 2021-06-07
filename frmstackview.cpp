#include "frmstackview.h"
#include "ui_frmstackview.h"
#include "bms_model.h"
#include "bmscollector.h"
#include "bms_bmudevice.h"
#include "bms_bcudevice.h"
#include "bms_svidevice.h"
#include "bms_stack.h"
#include "bms_system.h"
#include "inputwin.h"


frmStackView::frmStackView(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::frmStackView)
{
    ui->setupUi(this);
    batteryModel = new BMS_BatteryModel();
    stackModel = new BMS_StackModel();
    ui->lbInfo->setText("");
    ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
}

frmStackView::~frmStackView()
{
    delete ui;
}

void frmStackView::on_lineedit_focused(bool state)
{
    qDebug()<<Q_FUNC_INFO;
    FocusEditor *editor = (FocusEditor*)sender();
    InputWin *w = new InputWin;
    w->setAttribute(Qt::WA_DeleteOnClose);
    w->setDisplayContent(editor->text());
    connect(w,&InputWin::result,editor,&FocusEditor::setText);
    w->exec();
}

void frmStackView::setCollector(BMSCollector *collector)
{
    qDebug()<<Q_FUNC_INFO;
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
    ui->leTotalVoltage->setText(QString::number(stack->stackVoltage()/10.));
    ui->le_current->setText(QString::number(stack->stackCurrent()));

    ui->leSOC->setText(QString::number(stack->soc()));
    ui->leState->setText(stack->state());

    QByteArray dig_in = collector->currentSystem()->system->digitalInput();
    QByteArray dig_out = collector->currentSystem()->system->digitalOutput();
    QList<int> vs = collector->currentSystem()->system->vsource();

    // update UI
    ui->pbDigitalIn_0->setChecked((dig_in[0] & 0x01)==0x01?true:false);
    ui->pbDigitalIn_1->setChecked((dig_in[0] & 0x02)==0x02?true:false);

    ui->leVSourceIn_0->setText(QString::number(vs[0]));
    ui->leVSourceIn_1->setText(QString::number(vs[1]));


}

void frmStackView::on_pbSetVsource_0_clicked()
{
//    int value = ui->leVSourceSet_0->text().toInt();
//    collector->currentSystem()->setVoltageSource(0,value);
}

void frmStackView::on_pbSetVsource_1_clicked()
{
//    int value = ui->leVSourceSet_1->text().toInt();
//    collector->currentSystem()->setVoltageSource(1,value);
}
