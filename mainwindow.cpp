#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "stackinfo.h"
#include "frmhardwareconfig.h"
#include "frmstackconfig.h"
#include "frmhistoryview.h"
#include "frmeventview.h"
#include "bmscollector.h"
#include "bms_def.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    m_stackWin = new StackInfo();
    m_stackWin->hide();

    m_hardwareConfig = new frmHardwareConfig();
    m_hardwareConfig->hide();

    QString path;

    if(QSysInfo::productType().contains("win")){
        path = "./config/system.json";
    }
    else{
       path = QCoreApplication::applicationDirPath()+"/config/system.json";
    }

    collector = new BMSCollector();
    if(collector->loadConfig(path)){
        collector->connectServer(0);
        collector->readAllConfig();
    }
}

MainWindow::~MainWindow()
{
    delete m_stackWin;
    delete ui;
}

void MainWindow::on_pbSystemMonitor_clicked()
{
   // this->hide();
    m_stackWin->setCollector(collector);
    m_stackWin->show();
}

void MainWindow::on_pbHardwareInfo_clicked()
{
    m_hardwareConfig->show();
}

void MainWindow::on_pbSysConfig_clicked()
{
    frmStackConfig *config = new frmStackConfig("");
    config->show();
}

void MainWindow::on_pbViewEvent_clicked()
{
    frmEventView *evView = new frmEventView();
    evView->setModal(true);
    evView->show();
}

void MainWindow::on_pbSystemMonitor_2_clicked()
{
    frmHistoryView *hisView = new frmHistoryView();
    //hisView->setModal(true);
    //hisView->show();
}

void MainWindow::load_sys_config()
{
    collector = new BMSCollector();

}
