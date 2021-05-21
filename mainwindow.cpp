#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "stackinfo.h"
#include "frmhardwareconfig.h"
#include "frmstackconfig.h"
#include "frmhistoryview.h"
#include "frmeventview.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    m_stackWin = new StackInfo();
    m_stackWin->hide();

    m_hardwareConfig = new frmHardwareConfig();
    m_hardwareConfig->hide();
}

MainWindow::~MainWindow()
{
    delete m_stackWin;
    delete ui;
}

void MainWindow::on_pbSystemMonitor_clicked()
{
   // this->hide();
    m_stackWin->show();
}

void MainWindow::on_pbHardwareInfo_clicked()
{
    m_hardwareConfig->show();
}

void MainWindow::on_pbSysConfig_clicked()
{
    frmStackConfig *config = new frmStackConfig();
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
    hisView->setModal(true);
    hisView->show();
}
