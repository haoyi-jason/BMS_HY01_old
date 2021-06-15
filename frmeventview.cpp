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

    dummy();
}

frmEventView::~frmEventView()
{
    delete ui;
}

void frmEventView::on_pbClearAll_clicked()
{

}

void frmEventView::setLogFile(QString path)
{

}

void frmEventView::dummy()
{
    for(int i=0;i<10;i++){
        BMS_Event *evt = new BMS_Event;
        evt->m_evtID = i+1;
        evt->m_evtLevel = i*5;
        evt->m_isAlarm = true;
        evt->m_isWarning = false;
        evt->m_timeStamp = QDateTime::currentDateTime().addSecs(i*1000);
        evt->m_description = QString("Description %1").arg(i+1);
        m_evtModel->appendEvent(evt);
    }
    ui->tvEvents->viewport()->update();
}
