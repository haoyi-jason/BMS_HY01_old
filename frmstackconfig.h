#ifndef FRMSTACKCONFIG_H
#define FRMSTACKCONFIG_H

#include <QWidget>
#include "bms_localconfig.h"
#include "inputwin.h"
class QLineEdit;

namespace Ui {
class frmStackConfig;
}


class frmStackConfig : public QWidget
{
    Q_OBJECT

public:
    explicit frmStackConfig(QString configPath, QWidget *parent = nullptr);
    ~frmStackConfig();

private slots:
    void on_pbSetPath_clicked();

    void on_pbScan_clicked();

    void on_lwFiles_doubleClicked(const QModelIndex &index);

    void on_change_stack_view();

    void on_tableview_clicked(const QModelIndex &index);

private:
    void set_text_value(QLineEdit *edit, QString value);
    QString get_text_value(QLineEdit *edit);
    void set_config_to_ui();
    void load_config_from_ui();

private:
    Ui::frmStackConfig *ui;
    BMS_LocalConfig *m_localConfig;
    QString m_configPath;
    BMS_AlarmModel *alarmModel;
};

#endif // FRMSTACKCONFIG_H
