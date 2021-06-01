#ifndef FRMHARDWARECONFIG_H
#define FRMHARDWARECONFIG_H

#include <QWidget>

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


private slots:
    void on_system_config_ready();

    void on_system_data_ready();

    void update_collector();

    void on_pbListenSerialPort_clicked();

    void on_pbWriteSerialPort_clicked();

    void on_pbRelay_0_clicked();

    void on_pbRelay_1_clicked();

    void on_pbADCConfig_clicked();

    void on_pushButton_9_clicked();

private:
    Ui::frmHardwareConfig *ui;
    BMSCollector *m_collector;
};

#endif // FRMHARDWARECONFIG_H
