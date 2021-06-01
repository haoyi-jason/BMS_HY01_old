#include "collectorview.h"
#include "ui_collectorview.h"

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

CollectorView::CollectorView(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::CollectorView)
{
    ui->setupUi(this);
    m_StackWin = new frmStackView(this);
    m_HardwareWin = new frmHardwareConfig(this);
    m_HardwareWin->hide();
    m_HistWin = new frmHistoryView(this);
    m_HistWin->hide();

    ui->mainLayout->addWidget(m_StackWin);
    mainWidget = m_StackWin;

    m_collector = new BMSCollector();
    if(m_collector->loadConfig("./config/system.json")){
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
    QPushButton *btn = (QPushButton*)sender();
    if(btn->isChecked()){
        m_collector->connectServer(0);
        btn->setText("斷線");
    }else{
        m_collector->disconnectServer(0);
        btn->setText("連線");
    }
}
