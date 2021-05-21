#ifndef FRMSTACKCONFIG_H
#define FRMSTACKCONFIG_H

#include <QWidget>

namespace Ui {
class frmStackConfig;
}


class frmStackConfig : public QWidget
{
    Q_OBJECT

public:
    explicit frmStackConfig(QWidget *parent = nullptr);
    ~frmStackConfig();

private slots:
    void on_pbSetPath_clicked();

    void on_pbScan_clicked();

    void on_lwFiles_doubleClicked(const QModelIndex &index);


private:
    Ui::frmStackConfig *ui;
};

#endif // FRMSTACKCONFIG_H
