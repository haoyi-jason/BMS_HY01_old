#include "frmtest.h"
#include "ui_frmtest.h"

frmTest::frmTest(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::frmTest)
{
    ui->setupUi(this);
}

frmTest::~frmTest()
{
    delete ui;
}
