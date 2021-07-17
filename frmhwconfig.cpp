#include "frmhwconfig.h"
#include "ui_frmhwconfig.h"

frmHWConfig::frmHWConfig(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::frmHWConfig)
{
    ui->setupUi(this);
}

frmHWConfig::~frmHWConfig()
{
    delete ui;
}
