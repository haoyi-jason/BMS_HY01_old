#include "stackinfo.h"
#include "ui_stackinfo.h"
#include "bms_model.h"
#include "bmscollector.h"

StackInfo::StackInfo(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::StackInfo)
{
    ui->setupUi(this);
    stackModel = new BMS_StackModel();
    batteryModel = new BMS_BatteryModel();
    //dummyData();
    m_currentStackIndex = 0;
    collector = new BMSCollector();
    activeSystem = nullptr;
    QString ip = "127.0.0.1";
//    QString ip = "192.168.0.126";
    if(collector->addConnection(ip)){
        collector->readConfig(ip);
        //activeSystem = collector->currentSystem()->system;
        connect(collector,&BMSCollector::configReady,this,&StackInfo::on_system_config_ready);
        connect(collector,&BMSCollector::dataReceived,this,&StackInfo::on_system_data_ready);
    }

    ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
}

StackInfo::~StackInfo()
{
    delete ui;
}

void StackInfo::loadConfiguration(QString path)
{
    QString wp = path;
    if(path.isNull() || path.isEmpty()){
        wp = "./config";
    }

    // list files
    QStringList f_list = QDir(path).entryList(QStringList()<<"*.json",QDir::Files);
    if(f_list.size() == 0){
        return;
    }

    // parse file by file
    foreach (QString s, f_list) {
        QString fp = wp + "\\" + s;
        QFile f(fp);
        if(f.open(QIODevice::ReadOnly)){
            BMS_SystemInfo *info = new BMS_SystemInfo();
            if(info->Configuration(f.readAll())){
                m_bmsInfo.append(info);
            }
            else{
                delete info;
            }
            f.close();
        }
    }

}

void StackInfo::dummyData()
{

    for(int i=0;i<2;i++){
        BMS_StackInfo *stack_info = new BMS_StackInfo();
        for(int j=0;j<12;j++){
            BMS_BMUDevice *bat = new BMS_BMUDevice(12,5);
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

void StackInfo::on_pbNextStack_clicked()
{
    m_currentStackIndex++;
    if(m_currentStackIndex==stackModel->rowCount()){
        m_currentStackIndex = 0;
    }
    batteryModel->setStack(stackModel->findStack(m_currentStackIndex));
    //ui->tableView->setModel(batteryModel);
    ui->tableView->viewport()->update();
    updateStackInfo();
}

void StackInfo::on_pbPreviousStack_clicked()
{
    m_currentStackIndex--;
    if(m_currentStackIndex<0){
        m_currentStackIndex = stackModel->rowCount()-1;
    }
    batteryModel->setStack(stackModel->findStack(m_currentStackIndex));
    //ui->tableView->setModel(batteryModel);
    ui->tableView->viewport()->update();
    updateStackInfo();
}

void StackInfo::updateStackInfo()
{
    BMS_StackInfo *s = stackModel->findStack(m_currentStackIndex);
//    BMS_StackInfo *s = collector->currentSystem()->system->stacks();

    QString msg = "系統資訊 IP:";
    msg += activeSystem->connectionString;
    msg += "  總簇數:" + QString::number(activeSystem->stacks().count());
    msg +=   "目前為第 " + QString::number(m_currentStackIndex+1)+" 簇";
    ui->lbInfo->setText(msg);

    ui->leSOC->setText(QString::number(s->soc()));
    ui->leState->setText("IDLE");
    ui->leMaxCellVoltage->setText(QString::number(s->maxCellVoltage()));
    ui->le_maxTemp->setText(QString::number(s->maxStackTemperature()));
    ui->le_minTemp->setText(QString::number(s->minStackTemperature()));
    ui->le_minVoltage->setText(QString::number(s->minCellVoltage()));
    ui->le_current->setText(QString::number(s->stackCurrent()));
    ui->leTotalVoltage->setText(QString::number(s->stackVoltage()));

    //ui->le_maxDiff->setText(QString::number(s->));
}

void StackInfo::on_pbSystemNavi_clicked()
{
    activeSystem = collector->currentSystem()->system;;
    if(activeSystem != nullptr){
        ui->pbSystemNavi->setText(activeSystem->Alias);
    }
}

void StackInfo::on_system_config_ready()
{
    qDebug()<<"Config Ready";
    if(activeSystem == nullptr){
        activeSystem = collector->currentSystem()->system;
        if(activeSystem != nullptr){

            stackModel->setStack(activeSystem->stacks());
            batteryModel->setStack(stackModel->findStack(0));
            ui->tableView->setModel(batteryModel);
            ui->tableView_2->setModel(stackModel);
        }
    }
}

void StackInfo::on_system_data_ready()
{
    //qDebug()<<"Data Arrived";
    if(activeSystem != nullptr){
        ui->tableView->viewport()->update();
        updateStackInfo();
    }
}

void StackInfo::on_pbSwitchInfo_clicked()
{
    int c = ui->stackedWidget->currentIndex();
    c++;
    if(c >= ui->stackedWidget->count()){
        c = 0;
    }
    ui->stackedWidget->setCurrentIndex(c);
}