#ifndef FRMSTACKVIEW_H
#define FRMSTACKVIEW_H

#include <QWidget>
#include "bms_model.h"

namespace Ui {
class frmStackView;
}

class BMS_System;
class BMSCollector;
class QLabel;
class QPushButton;

//static QMap<QString, int> alarm_map = {"電池組"};
//static QString alarm_string[]={"電池組過壓","電池組欠壓","電池組過溫","電池組低溫","電芯過壓","電芯欠壓","電池過溫","電池低溫","BMU失聯","BCU失聯","SVI失聯"};
static QString alarm_string[]={"電池組過壓","電池組欠壓","電芯過壓","電芯欠壓","電池過溫","電池低溫","BMU失聯","SVI失聯"};

class frmStackView : public QWidget
{
    Q_OBJECT

public:
    explicit frmStackView(QWidget *parent = nullptr);
    ~frmStackView();

    void setCollector(BMSCollector *collector);
    void updateStackInfo();

public slots:
    void on_system_config_ready();
    void on_system_data_ready();
private slots:

    void on_pbSwitchInfo_clicked();

    void on_pbPreviousStack_clicked();

    void on_pbNextStack_clicked();

    void on_pbSetVsource_0_clicked();

    void on_pbSetVsource_1_clicked();

    void on_lineedit_focused(bool state);

    void on_clearAlarmClicked();

private:
    Ui::frmStackView *ui;
    BMS_StackModel *stackModel;
    BMS_BatteryModel *batteryModel;
    BMSCollector *collector;
    int m_currentStackIndex = 0;
    BMS_System *activeSystem = nullptr;
    QLabel *m_alarmLabels[12];
    QPushButton *m_btnClearAlarm;
};

#endif // FRMSTACKVIEW_H
