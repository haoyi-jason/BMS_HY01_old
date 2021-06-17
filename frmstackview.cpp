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
    ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);

    ui->label_6->setText(QString("最高電池溫度( %1C)").arg(QChar(0x00b0)));
    ui->label_8->setText(QString("最低電池溫度( %1C)").arg(QChar(0x00b0)));

    QGridLayout *gl = new QGridLayout;
    QWidget *w = new QWidget;
    for(int i=0;i<11;i++){
        m_alarmLabels[i] = new QLabel(alarm_string[i],w);
        m_alarmLabels[i]->setAlignment(Qt::AlignHCenter | Qt::AlignCenter);
        m_alarmLabels[i]->setFrameShape(QFrame::StyledPanel);
        m_alarmLabels[i]->setAutoFillBackground(true);
        gl->addWidget(m_alarmLabels[i],i%2,i/2);
    }

    m_btnClearAlarm = new QPushButton("清除\n告警",w);
    connect(m_btnClearAlarm,&QPushButton::clicked,this,&frmStackView::on_clearAlarmClicked);
    gl->addWidget(m_btnClearAlarm,0,6,2,1);

    w->setLayout(gl);
    ui->stackedWidget->addWidget(w);
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
    qDebug()<<Q_FUNC_INFO;
    this->collector = collector;
    QObject::connect(this->collector,&BMSCollector::configReady,this,&frmStackView::on_system_config_ready);
    QObject::connect(this->collector,&BMSCollector::dataReceived,this,&frmStackView::on_system_data_ready);
//    if(this->collector->currentSystem()->system != nullptr){
//        stackModel->setStack(this->collector->currentSystem()->system->stacks());
//        batteryModel->setStack(stackModel->findStack(0));
//        ui->tableView->setModel(batteryModel);
//    }
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
    ui->tableView->viewport()->update();
    updateStackInfo();
}

void frmStackView::on_pbNextStack_clicked()
{
    m_currentStackIndex++;
    if(m_currentStackIndex==collector->currentSystem()->system->Stacks){
        m_currentStackIndex = 0;
    }
    batteryModel->setStack(stackModel->findStack(m_currentStackIndex));

    ui->tableView->viewport()->update();
    updateStackInfo();
}

void frmStackView::updateStackInfo()
{
    QString message;
    message += QString("系統名稱：%1\r\n").arg(collector->currentSystem()->system->alias());
    message += QString("系統總簇數：%1\r\n").arg(collector->currentSystem()->system->Stacks);
    message += QString("目前在第 [ %1 ] 簇").arg(m_currentStackIndex+1);
    ui->lbInfo->setText(message);

    BMS_Stack *stack = stackModel->findStack(m_currentStackIndex);
    ui->leMaxCellVoltage->setText(QString::number(stack->maxCellVoltage()));
    ui->le_minVoltage->setText(QString::number(stack->minCellVoltage()));
    ui->le_maxTemp->setText(QString("%1").arg(stack->maxStackTemperature()/10.,5,'f',2,'0'));
    ui->le_minTemp->setText(QString("%1").arg(stack->minStackTemperature()/10.,5,'f',2,'0'));
    //ui->le_maxTemp->setText(QString::number(stack->maxStackTemperature()/10.));
//    ui->le_minTemp->setText(QString::number(stack->minStackTemperature()/10.));
    ui->leTotalVoltage->setText(QString("%1").arg(stack->stackVoltage()/10.,5,'f',2,'0'));
    ui->le_current->setText(QString("%1").arg(stack->stackCurrent()/10.,5,'f',2,'0'));

    ui->leSOC->setText(QString::number(stack->soc()));
    ui->leState->setText(stack->state());

    ui->le_maxDiff->setText(QString::number(stack->cellVoltDiff()));

    QByteArray dig_in = collector->currentSystem()->system->digitalInput();
    QByteArray dig_out = collector->currentSystem()->system->digitalOutput();
    QList<int> vs = collector->currentSystem()->system->vsource();

    // update UI
    ui->pbDigitalIn_0->setChecked((dig_in[0] & 0x01)==0x01?true:false);
    ui->pbDigitalIn_1->setChecked((dig_in[0] & 0x02)==0x02?true:false);

    ui->leVSourceIn_0->setText(QString::number(vs[0]));
    ui->leVSourceIn_1->setText(QString::number(vs[1]));

    quint32 alarm = collector->currentSystem()->system->alarmState();

    // feed alarm to ui
    for(int i=0;i<11;i++){
        QPalette p = m_alarmLabels[i]->palette();
        if(alarm & 0x01){
            p.setColor(m_alarmLabels[i]->backgroundRole(),Qt::red);
        }
        else{
            p.setColor(m_alarmLabels[i]->backgroundRole(),Qt::white);
        }
        m_alarmLabels[i]->setPalette(p);
        alarm >>= 1;
    }


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
