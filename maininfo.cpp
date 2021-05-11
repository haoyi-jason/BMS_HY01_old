#include "maininfo.h"
#include "ui_maininfo.h"

MainInfo::MainInfo(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MainInfo)
{
    ui->setupUi(this);
    ui->label_3->setProperty("type","1");
    this->style()->unpolish(ui->label_3);
    this->style()->polish((ui->label_3));
}

MainInfo::~MainInfo()
{
    delete ui;
}
