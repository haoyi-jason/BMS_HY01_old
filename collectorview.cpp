#include "collectorview.h"
#include "ui_collectorview.h"
#include <QtCore>
#include <QSysInfo>

#include "stackinfo.h"
#include "frmhardwareconfig.h"
#include "frmstackconfig.h"
#include "frmhistoryview.h"
#include "frmeventview.h"
#include "bmscollector.h"
#include "bms_def.h"
#include "frmtest.h"
#include "frmstackview.h"
#include "frmhistoryview.h"
#include "inputwin.h"

CollectorView::CollectorView(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::CollectorView)
{
    ui->setupUi(this);


    qDebug()<<"System Type:"<<QSysInfo::productType();
    m_StackWin = new frmStackView(this);
    m_HardwareWin = new frmHardwareConfig(this);
    m_HardwareWin->hide();
    m_HistWin = new frmHistoryView(this);
    m_HistWin->hide();

    ui->mainLayout->addWidget(m_StackWin);
    mainWidget = m_StackWin;
    QString path;
    if(QSysInfo::productType().contains("win")){
        path = "./config/system.json";
    }
    else{
       path = QCoreApplication::applicationDirPath()+"/config/system.json";
    }
    m_collector = new BMSCollector();
    if(m_collector->loadConfig(path)){
        m_collector->connectServer(-1);
        //m_collector->readAllConfig();
        m_StackWin->setCollector(m_collector);
        m_HardwareWin->setCollector(m_collector);
    }
}

CollectorView::~CollectorView()
{
    delete ui;
}

void CollectorView::hideWindows()
{
    m_StackWin->hide();
    m_HardwareWin->hide();
    m_HistWin->hide();
}

void CollectorView::on_pbStackView_clicked()
{
    hideWindows();
    if(mainWidget != nullptr){
        ui->mainLayout->removeWidget(mainWidget);
    }
    mainWidget = m_StackWin;
    ui->mainLayout->addWidget(mainWidget);
    mainWidget->show();
}

void CollectorView::on_pbHardwareView_clicked()
{
    hideWindows();
    if(mainWidget != nullptr){
        mainWidget->hide();
        ui->mainLayout->removeWidget(mainWidget);
    }
    mainWidget = m_HardwareWin;
    ui->mainLayout->addWidget(mainWidget);
    mainWidget->show();
}

void CollectorView::on_pbBatHistory_clicked()
{
    hideWindows();
    if(mainWidget != nullptr){
        mainWidget->hide();
        ui->mainLayout->removeWidget(mainWidget);
    }
    mainWidget = m_HistWin;
    ui->mainLayout->addWidget(mainWidget);
    mainWidget->show();
}

void CollectorView::on_pbSystemNavi_clicked()
{
    // process to standalone system
    QProcess *proc = new QProcess();

    QPushButton *btn = (QPushButton*)sender();
    if(btn->isChecked()){
        QString cmd = "/opt/BMS_Controller/bin/BMS_Controller";
        proc->start(cmd);
        //QProcess::startDetached(cmd);
        //proc->setProgram(cmd);
        //proc->startDetached();
//        proc->start("sh",QStringList()<<" -c"<<cmd);
//        proc->waitForFinished();
        qDebug()<<"Proc Result 3:"<<proc->readAll();
        QThread::sleep(1);
        m_collector->connectServer(0);
        btn->setText("斷線");

        m_HistWin->rootPath(m_collector->currentSystem()->logPath);
    }else{
        m_collector->disconnectServer(0);
        btn->setText("連線");

        QString cmd = "pkill BMS_Controller";
        proc->start(cmd);
//        proc->start("sh",QStringList()<<" -c"<<cmd);
        proc->waitForFinished();
        qDebug()<<"Proc Result:"<<proc->readAll();
    }
}
