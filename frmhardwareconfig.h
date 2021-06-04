#ifndef FRMHARDWARECONFIG_H
#define FRMHARDWARECONFIG_H

#include <QWidget>
#include "inputwin.h"

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
    void on_pushButton_9_clicked(bool checked);

private:
    Ui::frmHardwareConfig *ui;
    BMSCollector *m_collector;
};

#endif // FRMHARDWARECONFIG_H
