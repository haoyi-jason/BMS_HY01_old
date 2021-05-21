#include "frmeventview.h"
#include "ui_frmeventview.h"

frmEventView::frmEventView(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::frmEventView)
{
    ui->setupUi(this);
}

frmEventView::~frmEventView()
{
    delete ui;
}
