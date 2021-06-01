#ifndef FRMSTACKVIEW_H
#define FRMSTACKVIEW_H

#include <QWidget>
#include "bms_model.h"

namespace Ui {
class frmStackView;
}

class BMS_System;
class BMSCollector;

class frmStackView : public QWidget
{
    Q_OBJECT

public:
    explicit frmStackView(QWidget *parent = nullptr);
    ~frmStackView();

    void setCollector(BMSCollector *collector);
    void updateStackInfo();
private slots:
    void on_system_config_ready();
    void on_system_data_ready();

    void on_pbSwitchInfo_clicked();

    void on_pbPreviousStack_clicked();

    void on_pbNextStack_clicked();

    void on_pbSetVsource_0_clicked();

    void on_pbSetVsource_1_clicked();

private:
    Ui::frmStackView *ui;
    BMS_StackModel *stackModel;
    BMS_BatteryModel *batteryModel;
    BMSCollector *collector;
    int m_currentStackIndex = 0;
    BMS_System *activeSystem = nullptr;
};

#endif // FRMSTACKVIEW_H
