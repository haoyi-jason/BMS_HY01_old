#include "frmhistoryview.h"
#include "ui_frmhistoryview.h"
#include <QFileSystemModel>
#include "bms_model.h"
#include "bms_def.h"
#include "bms_bmudevice.h"
#include "bms_bcudevice.h"
#include "bms_svidevice.h"
#include "bms_stack.h"
#include "bms_system.h"
#include <QSysInfo>

frmHistoryView::frmHistoryView(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::frmHistoryView)
{
    ui->setupUi(this);

    model = new QFileSystemModel(this);
    batModel = new BMS_BatteryModel(this);
    stackModel = new BMS_StackModel(this);

    model->setRootPath(QDir::currentPath());
    ui->listView->setModel(model);

}

void frmHistoryView::rootPath(QString path)
{
    m_rootPath = path;
    ui->listView->setRootIndex(model->index(m_rootPath));
    qDebug()<<Q_FUNC_INFO<<" Set rootpath to :"<<path;
}

frmHistoryView::~frmHistoryView()
{
    delete ui;
}

void frmHistoryView::on_listView_clicked(const QModelIndex &index)
{
//    QString fname = "./log/192.168.0.102/"+model->fileName(index);

    QString fname = m_rootPath + "/" +model->fileName(index);
    BMS_System *sys = new BMS_System();

    QFile f(fname);
    if(f.open(QIODevice::ReadOnly)){
        QDataStream ds(&f);
        int sz;
        ds >> sz; // file size truncated, should be solved later...
        ds >> sys;
        stackModel->setStack(sys->stacks());
        batModel->setStack(stackModel->findStack(0));
        ui->tableView->setModel(batModel);
        ui->tableView->viewport()->update();
    }

}
