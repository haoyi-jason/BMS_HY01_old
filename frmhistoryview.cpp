#include "frmhistoryview.h"
#include "ui_frmhistoryview.h"
#include <QFileSystemModel>
#include "bms_model.h"
#include "bms_def.h"
#include "bms_bmudevice.h"
#include "bms_bcudevice.h"
#include "bms_svidevice.h"
#include "bms_stack.h"
#include "bms_system.h"
#include <QSysInfo>

frmHistoryView::frmHistoryView(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::frmHistoryView)
{
    ui->setupUi(this);

    model = new QFileSystemModel(this);
    batteryModel = new BMS_BatteryModel(this);
    stackModel = new BMS_StackModel(this);

    model->setRootPath(QDir::currentPath());
    ui->listView->setModel(model);

}

void frmHistoryView::rootPath(QString path)
{
    m_rootPath = path;
    ui->listView->setRootIndex(model->index(m_rootPath));
    qDebug()<<Q_FUNC_INFO<<" Set rootpath to :"<<path;
}

frmHistoryView::~frmHistoryView()
{
    delete ui;
}

void frmHistoryView::on_listView_clicked(const QModelIndex &index)
{
//    QString fname = "./log/192.168.0.102/"+model->fileName(index);

    QString fname = m_rootPath + "/" +model->fileName(index);
    BMS_System *sys = new BMS_System();

    QFile f(fname);
    if(f.open(QIODevice::ReadOnly)){
        QDataStream ds(&f);
        int sz;
        ds >> sz; // file size truncated, should be solved later...
        ds >> sys;
        stackModel->setStack(sys->stacks());
        batteryModel->setStack(stackModel->findStack(0));
        ui->tableView->setModel(batteryModel);
        ui->tableView->viewport()->update();
    }

}

void frmHistoryView::on_pbPreviousStack_clicked()
{
    m_currentStackIndex--;
    if(m_currentStackIndex<0){
        m_currentStackIndex = stackModel->rowCount()-1;
    }
    batteryModel->setStack(stackModel->findStack(m_currentStackIndex));
    ui->tableView->viewport()->update();
    updateStackInfo();
}

void frmHistoryView::on_pbNextStack_clicked()
{
    m_currentStackIndex++;
    if(m_currentStackIndex==stackModel->rowCount()){
        m_currentStackIndex = 0;
    }
    batteryModel->setStack(stackModel->findStack(m_currentStackIndex));
    ui->tableView->viewport()->update();
    updateStackInfo();
}

void frmHistoryView::updateStackInfo()
{
//    QString message;
//    message += QString("系統名稱：%1\r\n").arg(collector->currentSystem()->system->alias());
//    message += QString("系統總簇數：%1\r\n").arg(collector->currentSystem()->system->Stacks);
//    message += QString("目前在第 [ %1 ] 簇").arg(m_currentStackIndex+1);
    //ui->lbInfo->setText(message);

    BMS_StackInfo *stack = stackModel->findStack(m_currentStackIndex);
    ui->leMaxCellVoltage->setText(QString::number(stack->maxCellVoltage()));
    ui->le_minVoltage->setText(QString::number(stack->minCellVoltage()));
    ui->le_maxTemp->setText(QString::number(stack->maxStackTemperature()));
    ui->le_minTemp->setText(QString::number(stack->minStackTemperature()));
    ui->leTotalVoltage->setText(QString::number(stack->stackVoltage()/10.));
    ui->le_current->setText(QString::number(stack->stackCurrent()));


}

void frmHistoryView::on_listView_doubleClicked(const QModelIndex &index)
{

}
