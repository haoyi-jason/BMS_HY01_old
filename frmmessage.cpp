#include "frmmessage.h"
#include "ui_frmmessage.h"
#include <QDebug>

frmMessage::frmMessage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::frmMessage)
{
    ui->setupUi(this);
}

frmMessage::~frmMessage()
{
    delete ui;
}

void frmMessage::on_pbOK_clicked()
{
    emit ok();
}

void frmMessage::on_pbCancel_clicked()
{
//    qDebug()<<Q_FUNC_INFO;
    emit cancel();
}

void frmMessage::setText(QString title, QString content)
{
    ui->lbTitle->setText(title);
    ui->lbContent->setText(content);
}
