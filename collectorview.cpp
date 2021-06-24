#include "collectorview.h"
#include "ui_collectorview.h"
#include <QtCore>
#include <QSysInfo>
#include <QMessageBox>

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

#include "loginvalid.h"
#include "frmeventview.h"

#include "bms_bmudevice.h"
#include "bms_bcudevice.h"
#include "bms_svidevice.h"
#include "bms_stack.h"
#include "bms_system.h"

CollectorView::CollectorView(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::CollectorView)
{
    ui->setupUi(this);
    hide();

    qDebug()<<"System Type:"<<QSysInfo::productType();
    qDebug()<<QString("Width:%1, Height%2").arg(this->width()).arg(this->height());
    m_StackWin = new frmStackView(this);
    m_HardwareWin = new frmHardwareConfig();
    m_HardwareWin->hide();
    m_HistWin = new frmHistoryView();
    m_HistWin->hide();
    m_evtView = new frmEventView();
    m_evtView->hide();

    ui->mainLayout->addWidget(m_StackWin);
    mainWidget = m_StackWin;
    qDebug()<<QString("Width:%1, Height%2").arg(this->width()).arg(this->height());
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

    if(m_collector->loginPromote()){
        LoginValid *v = new LoginValid;
        v->setGeometry(this->x()+320,this->y()+320,v->width(),v->height());
        if(v->setFileName(path)){
            if(v->exec() == QDialog::Rejected){
                QMessageBox::warning(this,"錯誤!","密碼驗證失敗!");
                exit(-1);
            }
            else{
                m_userID = v->userID();
                qDebug()<<"User ID:"<< v->userID();
            }
        }
        else{
            QMessageBox::warning(this,"錯誤!","設定檔載入失敗!");
            exit(-1);
        }

        delete v;
    }
    else{ // hide hardware config in default
        m_userID = 0;
    }


    if(m_userID == 0){
        ui->pbHardwareView->setVisible(false);
    }else{
        ui->pbHardwareView->setVisible(true);
    }

    qDebug()<<QString("Width:%1, Height%2").arg(this->width()).arg(this->height());
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
    m_evtView->hide();
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
    m_HistWin->rootPath(m_collector->currentSystem()->system->logPath());
}
void CollectorView::on_pbEventView_clicked()
{
    hideWindows();
    if(mainWidget != nullptr){
        mainWidget->hide();
        ui->mainLayout->removeWidget(mainWidget);
    }
    mainWidget = m_evtView;
    ui->mainLayout->addWidget(mainWidget);
    mainWidget->show();

    QString path = m_collector->currentSystem()->system->logPath()+"/sys/events.log";
    m_evtView->setLogFile(path);
}

void CollectorView::on_pbSystemNavi_clicked()
{
    // process to standalone system
    QProcess *proc = new QProcess();

    QPushButton *btn = (QPushButton*)sender();
    if(btn->isChecked()){
        QString cmd;
        if(QSysInfo::productType().contains("win")){
        }
        else{
            // check if program is running
            proc->start("ps -C BMS_Controller");
            proc->waitForFinished();
            QStringList sl = QString(proc->readAll()).split("\n");
            if(sl.size() < 3){ // 1st : header, 2nd: pidxxx, 3rd blank
                //log("BMC_Controller not launched, try to launch");
                cmd = "/opt/BMS_Controller/bin/BMS_Controller";
                proc->start(cmd);
                qDebug()<<"Proc Result 3:"<<QString(proc->readAll());
                QThread::sleep(1);
            }
        }
        if(m_collector->connectServer(0)){
            //log("Start server ok");
            btn->setText("停止");
        }
        else{
            //log("Start server failed");
            btn->setChecked(false);
        }

    }else{
        //log("Disconnect from BMS Controller");
        m_collector->disconnectServer(0);
        btn->setText("連線");

//        QString cmd = "pkill BMS_Controller";
//        proc->start(cmd);
//        proc->start("sh",QStringList()<<" -c"<<cmd);
//        proc->waitForFinished();
//        qDebug()<<"Proc Result:"<<proc->readAll();
    }
}

void CollectorView::on_pbAuth_clicked()
{
    QString path;
    if(QSysInfo::productType().contains("win")){
        path = "./config/system.json";
    }
    else{
       path = QCoreApplication::applicationDirPath()+"/config/system.json";
    }

    LoginValid *v = new LoginValid;
    v->setGeometry(this->x()+320,this->y()+320,v->width(),v->height());
    if(v->setFileName(path)){
        if(v->exec() == QDialog::Accepted){
            if(v->userID() == 1){
                ui->pbHardwareView->setVisible(true);
            }
            else{
                ui->pbHardwareView->setVisible(false);
            }
        }
    }

    delete v;
}

