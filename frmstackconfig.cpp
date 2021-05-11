#include "frmstackconfig.h"
#include "ui_frmstackconfig.h"

frmStackConfig::frmStackConfig(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::frmStackConfig)
{
    ui->setupUi(this);
}

frmStackConfig::~frmStackConfig()
{
    delete ui;
}
