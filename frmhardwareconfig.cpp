#include "frmhardwareconfig.h"
#include "ui_frmhardwareconfig.h"
#include "bmscollector.h"
#include "bms_def.h"
#include "bms_bmudevice.h"
#include "bms_bcudevice.h"
#include "bms_svidevice.h"
#include "bms_stack.h"
#include "bms_system.h"
#include "inputwin.h"


frmHardwareConfig::frmHardwareConfig(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::frmHardwareConfig)
{
    ui->setupUi(this);
    connect(ui->leVSourceSet_0,&FocusEditor::focused,this,&frmHardwareConfig::on_lineedit_focused);
    connect(ui->leVSourceSet_1,&FocusEditor::focused,this,&frmHardwareConfig::on_lineedit_focused);
    connect(ui->leADCCfgValue,&FocusEditor::focused,this,&frmHardwareConfig::on_lineedit_focused);
    connect(ui->leRTUID,&FocusEditor::focused,this,&frmHardwareConfig::on_lineedit_focused);
    connect(ui->leTCPPort,&FocusEditor::focused,this,&frmHardwareConfig::on_lineedit_focused);
}

frmHardwareConfig::~frmHardwareConfig()
{
    delete ui;
}

void frmHardwareConfig::setCollector(BMSCollector *c)
{
    qDebug()<<Q_FUNC_INFO;
    m_collector = c;
    connect(m_collector,&BMSCollector::configReady,this,&frmHardwareConfig::on_system_config_ready);
    connect(m_collector,&BMSCollector::dataReceived,this,&frmHardwareConfig::on_system_data_ready);
}

void frmHardwareConfig::update_collector()
{
    QByteArray dig_in = m_collector->currentSystem()->system->digitalInput();
    QByteArray dig_out = m_collector->currentSystem()->system->digitalOutput();
    QList<int> vs = m_collector->currentSystem()->system->vsource();

    // update UI
    ui->lb_DI0->setText((dig_in[0] & 0x01)==0x01?"SET":"CLEAR");
    ui->lb_DI1->setText((dig_in[0] & 0x02)==0x02?"SET":"CLEAR");

    ui->leVSourceIn_0->setText(QString::number(vs[0]));
    ui->leVSourceIn_1->setText(QString::number(vs[1]));
}

void frmHardwareConfig::on_system_config_ready()
{
}

void frmHardwareConfig::on_system_data_ready()
{
    update_collector();
}

void frmHardwareConfig::on_pbListenSerialPort_clicked()
{
    QPushButton *btn = (QPushButton*)sender();
    if(btn->isChecked()){
        btn->setText("Close");
        ui->cbSerialPort->setEnabled(false);
        int b = ui->cbBaudrate->currentText().toInt();
        m_collector->currentSystem()->openSerialPort(ui->cbSerialPort->currentText(),b,1,1);
    }
    else{
        m_collector->currentSystem()->closeSerialPort(ui->cbSerialPort->currentText());
        ui->cbSerialPort->setEnabled(true);
    }
}

void frmHardwareConfig::on_pbWriteSerialPort_clicked()
{
    if(m_collector == nullptr) return;
    m_collector->currentSystem()->writeSerialPort(ui->cbSerialPort->currentText(),ui->leSeriaalWrite->text());
}

void frmHardwareConfig::on_pbRelay_0_clicked()
{
    if(m_collector == nullptr) return;
    if(ui->pbRelay_0->isChecked()){
        m_collector->currentSystem()->setDigitalOut(0,1);
    }
    else{
        m_collector->currentSystem()->setDigitalOut(0,0);
    }
}

void frmHardwareConfig::on_pbRelay_1_clicked()
{
    if(m_collector == nullptr) return;
    if(m_collector->currentSystem() == nullptr) return;
    if(ui->pbRelay_1->isChecked()){
        m_collector->currentSystem()->setDigitalOut(1,1);
    }
    else{
        m_collector->currentSystem()->setDigitalOut(1,0);
    }
}

void frmHardwareConfig::on_pbADCConfig_clicked()
{
    if(m_collector == nullptr) return;
    if(m_collector->currentSystem() == nullptr) return;
    QString command;
    command = QString("BCU:AIMAP:%1:%2:%3").arg(ui->cbADCChannel->currentIndex()).arg(ui->cbADCOption->currentIndex()).arg(ui->leADCCfgValue->text());
    m_collector->currentSystem()->writeCommand(command);
}

void frmHardwareConfig::on_pushButton_9_clicked()
{
    if(m_collector == nullptr) return;
    if(m_collector->currentSystem() == nullptr) return;
    int value = ui->leVSourceSet_0->text().toInt();
    m_collector->currentSystem()->setVoltageSource(0,value);

}

void frmHardwareConfig::on_pushButton_10_clicked()
{
    if(m_collector == nullptr) return;
    if(m_collector->currentSystem() == nullptr) return;
    int value = ui->leVSourceSet_1->text().toInt();
    m_collector->currentSystem()->setVoltageSource(1,value);
}

void frmHardwareConfig::on_pbSaveParam_clicked()
{
    if(m_collector == nullptr) return;
    if(m_collector->currentSystem() == nullptr) return;
    m_collector->currentSystem()->writeCommand("BCU:SAVE");
}

void frmHardwareConfig::on_pbEnVS0_clicked()
{
    if(m_collector == nullptr) return;
    if(m_collector->currentSystem() == nullptr) return;
    m_collector->currentSystem()->setVoltageSource(0,0);

}

void frmHardwareConfig::on_pbEnVS1_clicked()
{
    if(m_collector == nullptr) return;
    if(m_collector->currentSystem() == nullptr) return;
    m_collector->currentSystem()->setVoltageSource(1,0);

}

void frmHardwareConfig::on_lineedit_focused(bool state)
{
    qDebug()<<Q_FUNC_INFO;
    FocusEditor *editor = (FocusEditor*)sender();
    InputWin *w = new InputWin;
    w->setAttribute(Qt::WA_DeleteOnClose);
    w->setDisplayContent(editor->text());
    connect(w,&InputWin::result,editor,&FocusEditor::setText);
    w->exec();
}

void frmHardwareConfig::on_pushButton_9_clicked(bool checked)
{

}

void frmHardwareConfig::on_pbSVIConfig_clicked()
{
    if(m_collector == nullptr) return;
    if(m_collector->currentSystem() == nullptr) return;
    QString command;
    command = QString("SVI:AIMAP:%1:%2:%3").arg(ui->cbSVIChannel->currentIndex()).arg(ui->cbSVIOption->currentIndex()).arg(ui->leSVIValue->text());
    m_collector->currentSystem()->writeCommand(command);
}

void frmHardwareConfig::on_pbSaveSVI_clicked()
{
    if(m_collector == nullptr) return;
    if(m_collector->currentSystem() == nullptr) return;
    m_collector->currentSystem()->writeCommand("SVI:SAVE");
}

void frmHardwareConfig::on_pushButton_clicked()
{
    if(m_collector == nullptr) return;
    if(m_collector->currentSystem() == nullptr) return;
    QString command;
    command = QString("BMU:BV:%1:%2:%3:%4:%5").arg(ui->leBMUBV->text()).arg(ui->leBMUHV->text()).arg(ui->cbBalancing->isChecked()?"1":"0").arg(ui->leBMUBON->text()).arg(ui->leBMUBOFF->text());
    m_collector->currentSystem()->writeCommand(command);
}

void frmHardwareConfig::on_pbBMUBE_clicked()
{
    if(m_collector == nullptr) return;
    if(m_collector->currentSystem() == nullptr) return;
    QString command;
    command = QString("BMU:BE:1");
    m_collector->currentSystem()->writeCommand(command);
}

void frmHardwareConfig::on_pbBMUNBE_clicked()
{
    if(m_collector == nullptr) return;
    if(m_collector->currentSystem() == nullptr) return;
    QString command;
    command = QString("BMU:BE:0");
    m_collector->currentSystem()->writeCommand(command);
}
