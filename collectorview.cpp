#include "collectorview.h"
#include "ui_collectorview.h"
#include <QtCore>
#include <QSysInfo>
#include <QMessageBox>
#include <QTcpSocket>
#include <QMouseEvent>

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

#include <QTimer>

CollectorView::CollectorView(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::CollectorView)
{
    ui->setupUi(this);
    hide();

    // load from file
    QString path;

    if(QSysInfo::productType().contains("win")){
        path="d:/temp/bms/config/controller.json";
    }
    else{
        path = "/opt/bms/config/controller.json"; //-- change after Jul. 21'
    }

    BMS_LocalConfig config;
    config.load(path);


    //qDebug()<<"System Type:"<<QSysInfo::productType();
    //qDebug()<<QString("Width:%1, Height%2").arg(this->width()).arg(this->height());
    m_StackWin = new frmStackView();
    m_HardwareWin = new frmHardwareConfig(this);
    m_HistWin = new frmHistoryView();
    m_evtView = new frmEventView();
    m_messageBox = new frmMessage();
    m_HardwareWin->hide();
    m_HistWin->hide();
    m_evtView->hide();
    m_messageBox->hide();

    ui->mainLayout->addWidget(m_StackWin);
    mainWidget = m_StackWin;
//    m_StackWin->hide();;
//    ui->mainLayout->addWidget(m_HardwareWin);
//    m_HardwareWin->show();
//    mainWidget = m_HardwareWin;

    connect(m_HardwareWin,&frmHardwareConfig::restart_controller,this,&CollectorView::on_Issue_Restart_Controller);
    //qDebug()<<QString("Width:%1, Height%2").arg(this->width()).arg(this->height());
    //QString path;
    if(QSysInfo::productType().contains("win")){
        //path = "./config/system.json";
        path = "d:/temp/bms/config/system.json";
    }
    else{
       //path = QCoreApplication::applicationDirPath()+"/config/system.json";
        path = "/opt/bms/config/system.json";
    }

    m_collector = new BMSCollector();
    if(m_collector->loadConfig(path)){
        m_collector->connectServer(-1);
        //m_collector->readAllConfig();
        m_StackWin->setCollector(m_collector);
        m_HardwareWin->setCollector(m_collector);
        connect(m_collector,&BMSCollector::controllerOffline,this,&CollectorView::on_Controller_Offline);
    }

    if(m_collector->loginPromote()){
        LoginValid *v = new LoginValid;
        v->move(320,240);
        if(v->setFileName(path)){
            if(v->exec() == QDialog::Rejected){
                QMessageBox::warning(this,"錯誤!","密碼驗證失敗!");
                //exit(-1);
            }
            else{
                m_userID = v->userID();
                qDebug()<<"User ID:"<< v->userID();
            }
        }
        else{
            QMessageBox::warning(this,"錯誤!","設定檔載入失敗!");
            //exit(-1);
        }

        delete v;
    }
    else{ // hide hardware config in default
        m_userID = 0;
    }

    if(config.system.ConfigReady){
        if( config.system.AdminLogin)
            m_userID = 1;
        setBacklightDelay(config.system.BacklightOffDelay.toInt());
    }
    else{
        m_BacklightShutdownSec = 600;
        m_enableBacklightOff = true;
    }

    //m_userID = 1;
    switchUser();
//    if(m_userID == 0){
        ui->pbBatHistory->setVisible(false);
//        ui->pbHardwareView->setVisible(false);
//        //m_StackWin->showClearAlarm(false);
//        m_evtView->showClearEvent(false);
//    }else{
        ui->pbBatHistory->setVisible(false);
//        ui->pbHardwareView->setVisible(true);
//        //m_StackWin->showClearAlarm(true);
//        m_evtView->showClearEvent(true);
//    }

    if(!QSysInfo().productType().contains("win")){
        setWindowFlags(Qt::Window | Qt::CustomizeWindowHint | Qt::FramelessWindowHint);
        showFullScreen();
    }

    m_logValid = new LoginValid(this);
    connect(m_logValid,&LoginValid::accepted,this,&CollectorView::auth_accept);
    connect(m_logValid,&LoginValid::rejected,this,&CollectorView::auth_reject);

    m_idleTimer = new QTimer();
    //m_idleTimer->setSingleShot(true);
    connect(m_idleTimer,&QTimer::timeout,this,&CollectorView::on_Idle);
    m_idleTimer->start(1000);

 //   qDebug()<<QString("Width:%1, Height%2").arg(this->width()).arg(this->height());
    m_rtcLabel = new QLabel("yyyy/MM/dd hh:mm:ss");
    m_information = new QLabel;
    ui->statusbar->addPermanentWidget(m_rtcLabel);
    ui->statusbar->addWidget(m_information);

    QProcess *proc = new QProcess;
    QString cmd;
    cmd = QString("/bin/sh -c \"echo 0 > /sys/class/backlight/backlight-lvds/bl_power\"");
    proc->execute(cmd);
    proc->waitForFinished();
    m_TimeToShutdownScreen = 0;

    if(config.system.ConfigReady){
        //qDebug()<<"Config ready";
        if(config.system.AutoConnect){
            //on_pbSystemNavi_clicked();
            ui->pbSystemNavi->setChecked(true);
            QTimer::singleShot(1000,ui->pbSystemNavi,[this]{ui->pbSystemNavi->clicked();});
            //QTimer::singleShot(5000,ui->pbSystemNavi,[this]{ui->pbSystemNavi->clicked();});
        }
    }

    connect(m_HardwareWin,&frmHardwareConfig::connect_controller,this,&CollectorView::connect_controller);
    connect(m_HardwareWin,&frmHardwareConfig::query_controller_restart,this,&CollectorView::issue_controller_restart);
    connect(m_HardwareWin,&frmHardwareConfig::setBacklightDelay,this,&CollectorView::setBacklightDelay);

    connect(m_messageBox,&frmMessage::ok,this,&CollectorView::handleMsgOk);
    connect(m_messageBox,&frmMessage::cancel,this,&CollectorView::handleMsgCancel);

    m_client = new QTcpSocket();
}

void CollectorView::connect_controller(bool enable, int timeout_ms )
{
    m_information->setText("Try to connect to BMS Controller");
    //qDebug()<<Q_FUNC_INFO<<ui->pbSystemNavi->isChecked();
    ui->pbSystemNavi->setChecked(enable);
    //qDebug()<<Q_FUNC_INFO<<ui->pbSystemNavi->isChecked();
    QTimer::singleShot(timeout_ms,ui->pbSystemNavi,[this]{ui->pbSystemNavi->clicked();});
}

void CollectorView::issue_controller_restart()
{
    showMessage("訊息","設定已修改, 是否重新啟動BMS主程式?");
    pendingAction = 1;
}

CollectorView::~CollectorView()
{
    delete ui;
}

void CollectorView::on_Controller_Offline()
{
    if(pendingAction != 1){
        m_information->setText("BMS Controller Disonnected");
        // current only handle single controller connection
        if(ui->pbSystemNavi->isChecked()){ // shuld be
            if(restartController){
                connect_controller(true,3000);
            }
            else{
                m_collector->disconnectServer(0);
                ui->pbSystemNavi->setChecked(false);
                ui->pbSystemNavi->setText("連線");
            }
            //QMessageBox::information(this,"資訊","控制系統斷線, 請重新開機");
        }
    }
}

void CollectorView::on_Idle()
{
    m_rtcLabel->setText(QDateTime::currentDateTime().toString("yyyy/MM/dd hh:mm:ss"));
    if(!m_enableBacklightOff) return;

    if(m_TimeToShutdownScreen < m_BacklightShutdownSec){
        m_TimeToShutdownScreen++;
        if(m_TimeToShutdownScreen == m_BacklightShutdownSec){
            qDebug()<<Q_FUNC_INFO << "shutdown backlight";
            QProcess *proc = new QProcess;
            QString cmd;
            cmd = QString("/bin/sh -c \"echo 1 > /sys/class/backlight/backlight-lvds/bl_power\"");
            proc->execute(cmd);
            proc->waitForFinished();
        }
    }
}


void CollectorView::hideWindows()
{
    m_StackWin->hide();
    m_HardwareWin->hide();
    m_HistWin->hide();
    m_evtView->hide();
    m_messageBox->hide();
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
    m_HardwareWin->refresh_version_string();
    mainWidget->show();
}

void CollectorView::showMessage(QString title, QString content)
{
    hideWindows();
    if(mainWidget != nullptr){
        mainWidget->hide();
        lastWidget = mainWidget;
        ui->mainLayout->removeWidget(mainWidget);
    }
    mainWidget = m_messageBox;
    m_messageBox->setText(title,content);
    ui->mainLayout->addWidget(mainWidget);
    mainWidget->show();
}

void CollectorView::handleMsgCancel()
{
    qDebug()<<Q_FUNC_INFO;
    hideWindows();
    if(mainWidget != nullptr){
        mainWidget->hide();
        ui->mainLayout->removeWidget(mainWidget);
    }
    mainWidget = lastWidget;
    lastWidget = nullptr;
    ui->mainLayout->addWidget(mainWidget);
    mainWidget->show();

    pendingAction = -1;
}

void CollectorView::handleMsgOk()
{
    qDebug()<<Q_FUNC_INFO;
    hideWindows();
    if(mainWidget != nullptr){
        mainWidget->hide();
        ui->mainLayout->removeWidget(mainWidget);
    }
    mainWidget = lastWidget;
    lastWidget = nullptr;
    ui->mainLayout->addWidget(mainWidget);
    mainWidget->show();

    if(pendingAction == 1){ // restart controller
        restartController = true;
//        qDebug()<<"Restart controller";
        on_Issue_Restart_Controller();
//        connect_controller(true,3000);
        pendingAction = -1;
    }
}

void CollectorView::on_pbBatHistory_clicked()
{
//    hideWindows();
//    if(mainWidget != nullptr){
//        mainWidget->hide();
//        ui->mainLayout->removeWidget(mainWidget);
//    }
//    mainWidget = m_HistWin;
//    ui->mainLayout->addWidget(mainWidget);
//    mainWidget->show();
//    if(m_collector->currentSystem() != nullptr && m_collector->currentSystem()->system != nullptr){
//        m_HistWin->rootPath(m_collector->currentSystem()->system->logPath());
//    }
//    if(QMessageBox::question(this,"Confirm","OK to terminate",QMessageBox::Ok,QMessageBox::Cancel) == QMessageBox::Ok){
//        exit(-1);
//    }
    if(m_client == nullptr)
    {
        m_client = new QTcpSocket;
    }

    if(m_client->isOpen()){
        m_client->close();
    }
    else{
        m_client->connectToHost(QHostAddress("127.0.0.1"),5329);
    }
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

    if(m_collector->currentSystem() != nullptr && m_collector->currentSystem()->system != nullptr){
        //QString path = m_collector->currentSystem()->system->logPath()+"/sys/events.log";
        QString path,recPath;
        if(QSysInfo::productType().contains("win")){
            path = "d:/temp/bms/log/sys/events.log";
            recPath = "d:/temp/bms/log/record/eventLog.csv";
        }
        else{
            path = "/opt/bms/log/sys/events.log";
            recPath = "/opt/bms/log/record/eventLog.csv";
        }
        m_evtView->setLogFile(path,recPath);
    }
}

void CollectorView::on_pbSystemNavi_clicked()
{
    // process to standalone system
    QProcess *proc = new QProcess();

    QPushButton *btn = (QPushButton*)sender();
    qDebug()<<"Is checked:"<<btn->isChecked();
    if(btn->isChecked()){
        QString cmd;
        if(QSysInfo::productType().contains("win")){

        }
        else{
            // check if program is running
            proc->start("ps -C BMS_Controller");
            proc->waitForFinished();
            QStringList sl = QString(proc->readAll()).split("\n");
            //qDebug()<<"Try to launch controller:"<<sl;
            if(sl.size() < 3){ // 1st : header, 2nd: pidxxx, 3rd blank
                //log("BMC_Controller not launched, try to launch");
                cmd = "systemctl start bms_controller";
                proc->execute(cmd);
                proc->waitForFinished();
                //qDebug()<<"Proc Result 3:"<<QString(proc->readAll());
                QThread::sleep(1);
                //QMessageBox::information(this,"Info","控制系統未啟動, 請重新開機");
                //m_information->setText("控制系統未啟動, 請重新開機");
                //btn->setText("連線");
                //btn->setChecked(false);

            }
        }
        if(m_collector->connectServer(0)){
            //log("Start server ok");
            btn->setText("停止");
            m_information->setText("BMS Controller Connect OK");
        }
        else{
            //log("Start server failed");
            btn->setText("連線");
            btn->setChecked(false);
            m_information->setText("BMS Controller Connect Failed");
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
    proc->deleteLater();
}

void CollectorView::on_pbAuth_clicked()
{
    QString path;
    if(QSysInfo::productType().contains("win")){
        path = "./config/system.json";
    }
    else{
       //path = QCoreApplication::applicationDirPath()+"/config/system.json";
        path = "/opt/bms/config/system.json";
    }

    if(m_userID == 1){
        m_userID = 0;
        switchUser();
    }
    else{
        if(m_logValid->setFileName(path)){
            //Qt::WindowFlags flags = m_logValid->windowFlags();
            m_logValid->hide();
            //m_logValid->setWindowFlags(flags | Qt::WindowStaysOnTopHint);
            m_logValid->show();
        }
    }
}

void CollectorView::switchUser()
{
    if(m_userID == 1){
        ui->pbHardwareView->setVisible(true);
        ui->pbBatHistory->setVisible(true);
        ui->pbSystemNavi->setVisible(true);
//        m_StackWin->showClearAlarm(true);
        m_evtView->showClearEvent(true);
    }
    else{
        ui->pbHardwareView->setVisible(false);
        ui->pbBatHistory->setVisible(false);
        ui->pbSystemNavi->setVisible(false);
//        m_StackWin->showClearAlarm(false);
        m_evtView->showClearEvent(false);
    }
}

void CollectorView::auth_accept()
{
    m_userID = m_logValid->userID();
    switchUser();
    m_logValid->reset();

}

void CollectorView::auth_reject()
{
    m_logValid->hide();
    m_logValid->reset();
}

bool CollectorView::event(QEvent *event)
{
    //qDebug()<<Q_FUNC_INFO<<event->type();
    bool n = true;
    if(event->type() == QEvent::HoverMove){
        if(m_TimeToShutdownScreen == m_BacklightShutdownSec){
            qDebug()<<Q_FUNC_INFO << "Start backlight";
            QProcess *proc = new QProcess;
            QString cmd;
            cmd = QString("/bin/sh -c \"echo 0 > /sys/class/backlight/backlight-lvds/bl_power\"");
            proc->execute(cmd);
            proc->waitForFinished();
            n = false;
        }
        m_TimeToShutdownScreen = 0;
    }
    else if(event->type() == QEvent::MouseButtonDblClick){
        qDebug()<<Q_FUNC_INFO<<event->type();
        QMouseEvent *ev = static_cast<QMouseEvent*>(event);
        int xl = ui->label->x();
        int xr = xl + ui->label->width();
        int yt = ui->label->y();
        int yb = yt + ui->label->height();
        int cx = ev->pos().x();
        int cy = ev->pos().y();
        if((cx > xl) && (cx < xr) && (cy > yt)&&(cy < yb)){
            if(m_client == nullptr)
            {
                m_client = new QTcpSocket;
            }

            if(m_client->isOpen()){
                m_client->close();
            }
            else{
                m_client->connectToHost(QHostAddress("127.0.0.1"),5329);
            }
        }
    }
//    QProcess *proc = new QProcess;
//    QString cmd;
//        cmd = QString("/bin/sh -c \"echo %1 > /sys/class/backlight/backlight-lvds/brightness\"").arg(7);
//        proc->execute(cmd);
//        proc->waitForFinished();

//    delete proc;
    if(n){
        QMainWindow::event(event);
    }

}

void CollectorView::on_Issue_Restart_Controller()
{
    if(QSysInfo::productType().contains("win"))
        return;

    m_information->setText("Restarting BMS Controller");
    QProcess *proc = new QProcess;
    proc->start("systemctl restart bms_controller");
    proc->waitForFinished();
    QThread::msleep(500);
//    qDebug()<<Q_FUNC_INFO<<ui->pbSystemNavi->isChecked();
//    if(ui->pbSystemNavi->isChecked()){
//        m_collector->disconnectServer(0);
//        QThread::sleep(100);
//        QProcess *proc = new QProcess;
//        proc->start("systemctl restart bms_controller");
//        proc->waitForFinished();
//        QThread::sleep(100);
//        m_collector->connectServer(0);
//    }
}

void CollectorView::setBacklightDelay(unsigned int delay)
{
    if(delay == 0){
        m_enableBacklightOff = false;
    }
    else {
        m_enableBacklightOff = true;
        m_BacklightShutdownSec = delay;
    }
}
