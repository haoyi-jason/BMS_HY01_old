#ifndef FRMHISTORYVIEW_H
#define FRMHISTORYVIEW_H

#include <QWidget>

namespace Ui {
class frmHistoryView;
}

class QFileSystemModel;
class BMS_BatteryModel;
class BMS_StackModel;

class frmHistoryView : public QWidget
{
    Q_OBJECT

public:
    explicit frmHistoryView(QWidget *parent = nullptr);
    ~frmHistoryView();

private slots:
    void on_listView_clicked(const QModelIndex &index);

private:
    Ui::frmHistoryView *ui;
    QFileSystemModel *model=nullptr;
    BMS_BatteryModel *batModel = nullptr;
    BMS_StackModel *stackModel = nullptr;
};

#endif // FRMHISTORYVIEW_H
