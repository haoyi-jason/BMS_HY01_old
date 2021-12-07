#include "frmstackview.h"
#include "ui_frmstackview.h"
#include "bms_model.h"
#include "bmscollector.h"
#include "bms_bmudevice.h"
#include "bms_bcudevice.h"
#include "bms_svidevice.h"
#include "bms_stack.h"
#include "bms_system.h"
#include "inputwin.h"


frmStackView::frmStackView(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::frmStackView)
{
    ui->setupUi(this);
    batteryModel = new BMS_BatteryModel();
    stackModel = new BMS_StackModel();
    ui->lbInfo->setText("");
    ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableView->horizontalHeader()->setFrameShape(QFrame::VLine);

    ui->label_6->setText(QString("最高電池溫度( %1C)").arg(QChar(0x00b0)));
    ui->label_8->setText(QString("最低電池溫度( %1C)").arg(QChar(0x00b0)));

//    QGridLayout *gl = new QGridLayout;
    QHBoxLayout *hl = new QHBoxLayout;
    QWidget *w = ui->wgAlarm;
    for(int i=0;i<8;i++){
        m_alarmLabels[i] = new QLabel(alarm_string[i],w);
        m_alarmLabels[i]->setAlignment(Qt::AlignHCenter | Qt::AlignCenter);
        m_alarmLabels[i]->setFrameShape(QFrame::StyledPanel);
        m_alarmLabels[i]->setAutoFillBackground(true);
        hl->addWidget(m_alarmLabels[i]);
    }

    m_btnClearAlarm = new QPushButton("清除告警",w);
    m_btnClearAlarm->setSizePolicy(QSizePolicy::Preferred,QSizePolicy::Preferred);
    connect(m_btnClearAlarm,&QPushButton::clicked,this,&frmStackView::on_clearAlarmClicked);
    hl->addWidget(m_btnClearAlarm);

    w->setLayout(hl);
    //ui->stackedWidget->addWidget(w);
}

frmStackView::~frmStackView()
{
    delete ui;
}

void frmStackView::on_lineedit_focused(bool state)
{
    qDebug()<<Q_FUNC_INFO;
    FocusEditor *editor = (FocusEditor*)sender();
    InputWin *w = new InputWin;
    w->setAttribute(Qt::WA_DeleteOnClose);
    w->setDisplayContent(editor->text());
    connect(w,&InputWin::result,editor,&FocusEditor::setText);
    w->exec();
}

void frmStackView::setCollector(BMSCollector *collector)
{
//    qDebug()<<Q_FUNC_INFO;
    this->collector = collector;
    QObject::connect(this->collector,&BMSCollector::configReady,this,&frmStackView::on_system_config_ready);
    QObject::connect(this->collector,&BMSCollector::dataReceived,this,&frmStackView::on_system_data_ready);
}

void frmStackView::on_system_config_ready()
{
    if(collector->currentSystem() != nullptr){
        activeSystem = collector->currentSystem()->system;
    }
    else{
        activeSystem = nullptr;
    }
    if(activeSystem != nullptr){
        stackModel->setStack(activeSystem->stacks());
        batteryModel->setStack(stackModel->findStack(0));
        ui->tableView->setModel(batteryModel);
        m_currentStackIndex = 0;
    }
}

void frmStackView::on_system_data_ready()
{
    if(activeSystem == nullptr){
        if(collector->currentSystem() != nullptr){
            activeSystem = collector->currentSystem()->system;
        }
        if(activeSystem != nullptr){
            stackModel->setStack(activeSystem->stacks());
            batteryModel->setStack(stackModel->findStack(0));
            ui->tableView->setModel(batteryModel);
            m_currentStackIndex = 0;
        }
    }
    if(activeSystem != nullptr){
        if(m_currentStackIndex == collector->currentSystem()->system->minSID()){
            ushort bid = collector->currentSystem()->system->minBID();
            ushort cid = collector->currentSystem()->system->minCID();
            batteryModel->setMinInfo(bid,cid);
        }
        else{
            batteryModel->setMinInfo(-1,-1);
        }

        ui->tableView->viewport()->update();
        updateStackInfo();
    }

}

void frmStackView::on_pbSwitchInfo_clicked()
{
    int c = ui->stackedWidget->currentIndex();
    c++;
    if(c >= ui->stackedWidget->count()){
        c = 0;
    }
    ui->stackedWidget->setCurrentIndex(c);
}

void frmStackView::on_pbPreviousStack_clicked()
{
    m_currentStackIndex--;
    if(m_currentStackIndex<0){
        m_currentStackIndex = stackModel->rowCount()-1;
    }
    batteryModel->setStack(stackModel->findStack(m_currentStackIndex));
    batteryModel->headerDataChanged(Qt::Horizontal,0,stackModel->findStack(m_currentStackIndex)->batteries().count());
    ui->tableView->viewport()->update();
    updateStackInfo();
}

void frmStackView::on_pbNextStack_clicked()
{
    m_currentStackIndex++;
    if(m_currentStackIndex >= collector->currentSystem()->system->stacks().size()){
        m_currentStackIndex = 0;
    }
    batteryModel->setStack(stackModel->findStack(m_currentStackIndex));
    batteryModel->headerDataChanged(Qt::Horizontal,0,stackModel->findStack(m_currentStackIndex)->batteries().count());

    ui->tableView->viewport()->update();

    updateStackInfo();
}

void frmStackView::updateStackInfo()
{
    QString message;
    //message += QString("系統名稱：%1\r\n").arg(collector->currentSystem()->system->alias());
    message += QString("系統總簇數：%1\r\n").arg(collector->currentSystem()->system->stacks().size());
    message += QString("目前在第 [ %1 ] 簇\n").arg(m_currentStackIndex+1);

    int current = 0;
    foreach(BMS_Stack *s,collector->currentSystem()->system->stacks()){
        current += s->stackCurrent();
    }
    message += QString("總電流:%1 A\n").arg(current/10,5,'f',1,'0');

    ushort sid = collector->currentSystem()->system->minSID();
    ushort bid = collector->currentSystem()->system->minBID();
    ushort cid = collector->currentSystem()->system->minCID();

    if(cid < 30){
        message += QString("最低電壓:%1-%2-C%3\n").arg(sid+1).arg(bid+1).arg(cid+1);
    }
    else{
        message += QString("最低電壓:未知");
    }

    BMS_Stack *stack = stackModel->findStack(m_currentStackIndex);
    ui->leMaxCellVoltage->setText(QString("%1").arg(stack->maxCellVoltage()/1000.,5,'f',3,'0'));
//    ui->leMaxCellVoltage->setText(QString("%1").arg(collector->currentSystem()->system->->maxCellVoltage()/1000.,5,'f',3,'0'));
//    ui->le_minVoltage->setText(QString("%1").arg(stack->minCellVoltage()/1000.,5,'f',3,'0'));
    ui->le_minVoltage->setText(QString("%1").arg(collector->currentSystem()->system->miniCellVoltage()/1000.,5,'f',3,'0'));
    ui->le_maxTemp->setText(QString("%1").arg(stack->maxStackTemperature()/10.,4,'f',1,'0'));
    ui->le_minTemp->setText(QString("%1").arg(stack->minStackTemperature()/10.,4,'f',1,'0'));
    ui->leTotalVoltage->setText(QString("%1").arg(stack->stackVoltage()/10.,5,'f',1));
    ui->le_current->setText(QString("%1").arg(stack->stackCurrent()/10.,5,'f',1));

    ui->leSOC->setText(QString("%1").arg(stack->sviDevice()->soc(),5,'f',1,'0'));

    if(stack->stackCurrent() > 10){
        ui->leState->setText("充電");
    }
    else if(stack->stackCurrent() < -10){
        ui->leState->setText("放電");
    }
    else{
        ui->leState->setText("待機");
    }

    ui->le_maxDiff->setText(QString::number(stack->cellVoltDiff()));

    QByteArray dig_in = collector->currentSystem()->system->bcu()->getDigitalInput();
    QByteArray dig_out = collector->currentSystem()->system->bcu()->getDigitalOutput();
    QList<int> vs = collector->currentSystem()->system->bcu()->getWorkingCurrent();

    // update UI
//    ui->pbDigitalIn_0->setChecked((dig_in[0] & 0x01)==0x01?true:false);
//    ui->pbDigitalIn_1->setChecked((dig_in[0] & 0x02)==0x02?true:false);

//    ui->leVSourceIn_0->setText(QString::number(vs[0]));
//    ui->leVSourceIn_1->setText(QString::number(vs[1]));

    if((dig_out[0] & 0x01) == 0x00){
        ui->lbl_do0->setText("Warning OFF");
        QPalette p = ui->lbl_do0->palette();
        p.setColor(ui->lbl_do0->backgroundRole(),Qt::white);
        ui->lbl_do0->setPalette(p);
    }
    else{
        ui->lbl_do0->setText("Warning ON");
        QPalette p = ui->lbl_do0->palette();
        p.setColor(ui->lbl_do0->backgroundRole(),Qt::yellow);
        //p.setColor(ui->lbl_do0->foregroundRole(),Qt::green);
        ui->lbl_do0->setPalette(p);
    }
    if((dig_out[0] & 0x02) == 0x00){
        ui->lbl_do1->setText("Alarm OFF");
        QPalette p = ui->lbl_do1->palette();
        p.setColor(ui->lbl_do1->backgroundRole(),Qt::white);
        ui->lbl_do1->setPalette(p);
    }
    else{
        ui->lbl_do1->setText("Alarm ON");
        QPalette p = ui->lbl_do1->palette();
        p.setColor(ui->lbl_do1->backgroundRole(),Qt::red);
        ui->lbl_do1->setPalette(p);
    }


//    ui->lbl_do1->setText((dig_out[0] & 0x02)==0x02?"輸出[2]開啟":"輸出[2]關閉");
    //qDebug()<<"Digital Out:"<< QString::number(dig_out[0],16);

    quint32 state = collector->currentSystem()->system->alarmState();
    //qDebug()<<QString("Alarm Code:0x%1").arg(alarm,16);

    // feed alarm to ui
    quint32 warning = state & 0xffff;
    for(int i=0;i<8;i++){
        QPalette p = m_alarmLabels[i]->palette();
        if(warning & 0x01){
            p.setColor(m_alarmLabels[i]->backgroundRole(),Qt::yellow); // warning
        }
        else{
            p.setColor(m_alarmLabels[i]->backgroundRole(),Qt::white);
        }
        m_alarmLabels[i]->setPalette(p);
        warning >>= 1;
    }
    quint32 alarm = (state >> 16);
    for(int i=0;i<6;i++){
        QPalette p = m_alarmLabels[i]->palette();
        if(alarm & 0x01){
            p.setColor(m_alarmLabels[i]->backgroundRole(),Qt::red);
        }
        m_alarmLabels[i]->setPalette(p);
        alarm >>= 1;
    }

    ui->lbInfo->setText(message);


}

void frmStackView::on_pbSetVsource_0_clicked()
{
//    int value = ui->leVSourceSet_0->text().toInt();
//    collector->currentSystem()->setVoltageSource(0,value);
}

void frmStackView::on_pbSetVsource_1_clicked()
{
//    int value = ui->leVSourceSet_1->text().toInt();
//    collector->currentSystem()->setVoltageSource(1,value);
}

void frmStackView::on_clearAlarmClicked()
{
    if(collector == nullptr) return;
    if(collector->currentSystem() == nullptr) return;

    //collector->currentSystem()->system->clearAlarm();
    collector->currentSystem()->writeCommand("SYS:ALMRST");
}

void frmStackView::showClearAlarm(bool show)
{
    m_btnClearAlarm->setVisible(show);
}
