#ifndef FRMHARDWARECONFIG_H
#define FRMHARDWARECONFIG_H

#include <QWidget>
#include "inputwin.h"
#include "bms_localconfig.h"

namespace Ui {
class frmHardwareConfig;
}
class BMSCollector;

class frmHardwareConfig : public QWidget
{
    Q_OBJECT

public:
    explicit frmHardwareConfig(QWidget *parent = nullptr);
    ~frmHardwareConfig();
    void setCollector(BMSCollector *c);
    void setLocalConfig(QString cfgName);

public slots:
    void on_system_config_ready();

    void on_system_data_ready();

private slots:

    void update_collector();

    void on_pbListenSerialPort_clicked();

    void on_pbWriteSerialPort_clicked();

    void on_pbRelay_0_clicked();

    void on_pbRelay_1_clicked();

    void on_pbADCConfig_clicked();

    void on_pushButton_9_clicked();

    void on_pushButton_10_clicked();

    void on_pbSaveParam_clicked();

    void on_pbEnVS0_clicked();

    void on_pbEnVS1_clicked();

    void on_lineedit_focused(bool state);

    void on_lineedit_focused_ip(bool state);

    void on_lineedit_edited(QString text);

    void on_pushButton_9_clicked(bool checked);

    void on_pbSVIConfig_clicked();

    void on_pbSaveSVI_clicked();

    void on_pushButton_clicked();

    void on_pbBMUBE_clicked();

    void on_pbBMUNBE_clicked();

    void on_pbRestartController_clicked();

    void on_pbDisableController_clicked();

    void on_pbEnableController_clicked();

    void on_pbSetDateTime_clicked();

    void on_pbAlarmToSD_clicked();

    void on_pbRecordToSD_clicked();

    void on_pbLoadLocalSetting_clicked();

    void on_pbCellWarning_clicked();

    void on_pbCellAlarm_clicked();

    void on_pbBatteryWarning_clicked();

    void on_pbBatteryAlarm_clicked();

    void on_pbStackWarning_clicked();

    void on_pbStackAlarm_clicked();

    void on_pbSaveLocalConfig_clicked();

    void on_pbSOCWarning_clicked();

    void on_pbSOCAlarm_clicked();

    void on_pbSimSV_clicked();

    void on_pbSimSA_clicked();

    void on_pbSimCV_clicked();

    void on_pbSimPT_clicked();

    void on_pbSimReset_clicked();

    void on_comboBox_currentIndexChanged(int index);

    void on_pbSetSOC_clicked();

    void on_pbEnterLanMode_clicked();

    void on_pbSaveBL_clicked();

    void on_pbApplyNetwork_clicked();

signals:
    void restart_controller();
    void connect_controller(bool,int);
    void setBacklightDelay(unsigned int);
    void query_controller_restart();

private:
    void updateLocalSetting();
    void set_backlight(int brightness, bool off = false);
    void load_settings();


private:
    Ui::frmHardwareConfig *ui;
    BMSCollector *m_collector;
    BMS_LocalConfig localConfig;
    QString CurrentCriteria;

    InputWin *m_inputWin;
};

#endif // FRMHARDWARECONFIG_H
