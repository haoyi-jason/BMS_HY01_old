#include "frmeventview.h"
#include "ui_frmeventview.h"
#include "bms_model.h"

frmEventView::frmEventView(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::frmEventView)
{
    ui->setupUi(this);
    m_evtModel = new BMS_EventModel;
    ui->tvEvents->setModel(m_evtModel);
    ui->tvEvents->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);


//    ui->tvEvents->setColumnWidth(0,80);
//    ui->tvEvents->setColumnWidth(1,80);
//    ui->tvEvents->setColumnWidth(2,80);
//    ui->tvEvents->setColumnWidth(3,80);
//    ui->tvEvents->setColumnWidth(4,80);
//    ui->tvEvents->setColumnWidth(5,120);
//    ui->tvEvents->setColumnWidth(6,800);
//    ui->tvEvents->horizontalHeader()->setSectionResizeMode(6,QHeaderView::Stretch);
//    ui->tvEvents->horizontalHeader()->setsect

    //dummy();
}

frmEventView::~frmEventView()
{
    delete ui;
}

void frmEventView::on_pbClearAll_clicked()
{
    ui->tvEvents->setModel(nullptr);
    m_evtModel->clearEvents();
    QFile f(m_path);
    f.remove();
}

void frmEventView::setLogFile(QString path)
{
    QFile f(path);
    m_path = path;
    if(f.open(QIODevice::ReadOnly)){
        QTextStream ts(&f);//,QIODevice::ReadOnly | QIODevice::Text);
        ui->tvEvents->setModel(nullptr);
        m_evtModel->clearEvents();
        while(!ts.atEnd()){
            QString s = ts.readLine();
            BMS_Event *e = new BMS_Event;
            e->parse(s);
            m_evtModel->appendEvent(e);
        }
        f.close();
        ui->tvEvents->setModel(m_evtModel);
        ui->tvEvents->resizeColumnsToContents();
        ui->tvEvents->viewport()->update();

    }
}

void frmEventView::dummy()
{
    for(int i=0;i<10;i++){
        BMS_Event *evt = new BMS_Event;
        evt->m_evtID = i+1;
        evt->m_evtLevel = i*5;
        evt->m_isAlarm = true;
        evt->m_isWarning = false;
        evt->m_timeStamp = QDateTime::currentDateTime().addSecs(i*1000).toString("yyyyMMdd_hhmmss");
        evt->m_description = QString("Description %1").arg(i+1);
        m_evtModel->appendEvent(evt);
    }
    ui->tvEvents->viewport()->update();
}
