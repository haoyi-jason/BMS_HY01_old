#ifndef STACKINFO_H
#define STACKINFO_H

#include <QWidget>
#include "bms_model.h"

namespace Ui {
class StackInfo;
}

class StackInfo : public QWidget
{
    Q_OBJECT

public:
    explicit StackInfo(QWidget *parent = nullptr);
    ~StackInfo();

private slots:
    void on_tableView_2_activated(const QModelIndex &index);

    void on_tableView_2_clicked(const QModelIndex &index);

private:
    void dummyData();

private:
    Ui::StackInfo *ui;
    BMS_StackModel *stackModel;
    BMS_BatteryModel *batteryModel;
};

#endif // STACKINFO_H
