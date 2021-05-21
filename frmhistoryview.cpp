#include "frmhistoryview.h"
#include "ui_frmhistoryview.h"

frmHistoryView::frmHistoryView(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::frmHistoryView)
{
    ui->setupUi(this);
}

frmHistoryView::~frmHistoryView()
{
    delete ui;
}
