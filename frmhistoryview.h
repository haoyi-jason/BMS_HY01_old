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
    void rootPath(QString p);
private slots:
    void on_listView_clicked(const QModelIndex &index);

    void on_pbPreviousStack_clicked();

    void on_pbNextStack_clicked();

    void updateStackInfo();

    void on_listView_doubleClicked(const QModelIndex &index);

private:
    Ui::frmHistoryView *ui;
    QFileSystemModel *model=nullptr;
    BMS_BatteryModel *batteryModel = nullptr;
    BMS_StackModel *stackModel = nullptr;
    QString m_rootPath="";
    int m_currentStackIndex = 0;
};

#endif // FRMHISTORYVIEW_H
