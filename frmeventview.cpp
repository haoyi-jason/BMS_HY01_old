#include "frmeventview.h"
#include "ui_frmeventview.h"
#include "bms_model.h"
#include <QtCharts>

frmEventView::frmEventView(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::frmEventView)
{
    ui->setupUi(this);
    m_evtModel = new BMS_EventModel;
    ui->tvEvents->setModel(m_evtModel);
    ui->tvEvents->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->comboBox->hide();

   // QtCharts *c = new QtCharts(this);


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

void frmEventView::reload()
{
    QFile f(m_path);
    if(f.open(QIODevice::ReadOnly)){
        QTextStream ts(&f);//,QIODevice::ReadOnly | QIODevice::Text);
        ts.setCodec(QTextCodec::codecForName("Big5"));
        ui->tvEvents->setModel(nullptr);
        m_evtModel->clearEvents();
        if(m_totalPage == -1){
            int n = 0;
            ts.readLine(); // first row
            while(!ts.atEnd()){

                if(n<m_pageSize){
                    QString s = ts.readLine();
                    BMS_Event *e = new BMS_Event;
                    e->parse(s);
                    m_evtModel->appendEvent(e);
                }
                else{
                    ts.readLine();
                }
                n++;
            }
            f.close();
            m_currentPage = 0;
            m_totalPage = n/m_pageSize+1;
        }
        else{
            int byp = m_currentPage*m_pageSize+1;
            int n = 0;
            bool stop = false;
            while(!ts.atEnd() && !stop){
                if(n < byp){
                    ts.readLine();
                }
                else if(n < (byp + m_pageSize))
                {
                    QString s = ts.readLine();
                    BMS_Event *e = new BMS_Event;
                    e->parse(s);
                    m_evtModel->appendEvent(e);
                }
                else{
                    stop = true;
                }
                n++;
            }
        }
        m_evtModel->setPage(m_currentPage);
        ui->tvEvents->setModel(m_evtModel);
        //ui->tvEvents->resizeColumnsToContents();
        ui->tvEvents->viewport()->update();
    }
    f.close();
    if(m_totalPage>0){
        QString msg = QString("第%1頁/共%2頁").arg(m_currentPage+1).arg(m_totalPage);
        ui->lbPageInfo->setText(msg);
    }
    else{
        ui->lbPageInfo->setText("");
    }

}

void frmEventView::setLogFile(QString &path, QString recordPath)
{
    m_path = path;
    m_currentPage = 0;
    m_totalPage = -1;

    m_eventLogPath = recordPath;

    //ui->wCharg->loadDataFromFile(m_eventLogPath);
    reload();
}

void frmEventView::dummy()
{
//    for(int i=0;i<10;i++){
//        BMS_Event *evt = new BMS_Event;
//        evt->m_evtID = i+1;
//        evt->m_evtLevel = i*5;
//        evt->m_isAlarm = true;
//        evt->m_isWarning = false;
//        evt->m_timeStamp = QDateTime::currentDateTime().addSecs(i*1000).toString("yyyyMMdd_hhmmss");
//        evt->m_description = QString("Description %1").arg(i+1);
//        m_evtModel->appendEvent(evt);
//    }
//    ui->tvEvents->viewport()->update();
}

void frmEventView::on_pbPrevPage_clicked()
{
    if(m_totalPage > 0){
        m_currentPage--;
        if(m_currentPage<0)
            m_currentPage = m_totalPage -1;
        reload();
    }
}

void frmEventView::on_pbNextPage_clicked()
{
    if(m_totalPage > 0){
        m_currentPage++;
        if(m_currentPage>= m_totalPage)
            m_currentPage = 0;
        reload();
    }

}

void frmEventView::on_pbDeleteCurrent_clicked()
{
    reload();
}

void frmEventView::on_tvEvents_clicked(const QModelIndex &index)
{
    return;
    // load records
    int c = index.column();
    if(c == 6){
        QString data = m_evtModel->data(index).toString();
        qDebug()<<"Data:"<<data;
        // parse data for stack number
        QRegExp rx("S-(\\d*),(([0-9]*[.])?[0-9]+)V");
        QStringList sl;
        int pos = 0;
        while((pos = rx.indexIn(data,pos))!=-1){
            sl << rx.cap(1)<<rx.cap(2);
            pos += rx.matchedLength();
        }
        qDebug()<<"REGEXP:"<<sl;
        if(sl.size() == 0) return;
        int stack = sl[0].toInt();

        //ui->wCharg->showSeriesByStack(stack);
    }

}

void frmEventView::on_comboBox_currentIndexChanged(int index)
{
    return;
    int sec = 100;
    QComboBox *cbo = static_cast<QComboBox*>(sender());
    switch(cbo->currentIndex()){
    case 0: sec = 3600;break;
    case 1: sec = 28800;break;
    case 2: sec = 86400;break;
    default:sec = 3600; break;
    }

    //ui->wCharg->setWindowsWidth(sec);

}

void frmEventView::showClearEvent(bool show)
{
    ui->pbClearAll->setVisible(show);
}
