#include "frmhardwareconfig.h"
#include "ui_frmhardwareconfig.h"

frmHardwareConfig::frmHardwareConfig(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::frmHardwareConfig)
{
    ui->setupUi(this);
}

frmHardwareConfig::~frmHardwareConfig()
{
    delete ui;
}
