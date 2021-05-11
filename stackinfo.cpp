#include "stackinfo.h"
#include "ui_stackinfo.h"

StackInfo::StackInfo(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::StackInfo)
{
    ui->setupUi(this);
    dummyData();
}

StackInfo::~StackInfo()
{
    delete ui;
}

void StackInfo::dummyData()
{
    stackModel = new BMS_StackModel();
    batteryModel = new BMS_BatteryModel();

    for(int i=0;i<2;i++){
        BMS_StackInfo *stack_info = new BMS_StackInfo();
        for(int j=0;j<12;j++){
            BMS_BatteryInfo *bat = new BMS_BatteryInfo(12,5);
            QList<ushort> v,t;
            for(int k=0;k<12;k++){
                v.append(3000+QRandomGenerator::global()->bounded(500));
            }
            bat->cellVoltages(v);
            for(int k=0;k<5;k++){
                t.append(30+QRandomGenerator::global()->bounded(5));
            }
            bat->packTemperatures(t);
            stack_info->addBattery(bat);
        }
        stackModel->addStack(stack_info);
    }

    batteryModel->setStack(stackModel->findStack(0));
    ui->tableView->setModel(batteryModel);
    ui->tableView_2->setModel(stackModel);
}

void StackInfo::on_tableView_2_activated(const QModelIndex &index)
{
    int r = index.row();
    if(r < stackModel->rowCount()){
        batteryModel->setStack(stackModel->findStack(r));
        ui->tableView->setModel(batteryModel);
    }
}

void StackInfo::on_tableView_2_clicked(const QModelIndex &index)
{
    int r = index.row();
    if(r < stackModel->rowCount()){
        batteryModel->setStack(stackModel->findStack(r));
        //ui->tableView->setModel(batteryModel);
        ui->tableView->viewport()->update();
    }
}
