#ifndef FRMEVENTVIEW_H
#define FRMEVENTVIEW_H

#include <QDialog>

namespace Ui {
class frmEventView;
}

class BMS_EventModel;

class frmEventView : public QDialog
{
    Q_OBJECT

public:
    explicit frmEventView(QWidget *parent = nullptr);
    ~frmEventView();

public slots:
    void setLogFile(QString &path, QString recordPath = "");

private slots:
    void on_pbClearAll_clicked();
    void dummy();

    void on_pbPrevPage_clicked();

    void on_pbNextPage_clicked();

    void reload();

    void on_pbDeleteCurrent_clicked();

    void on_tvEvents_clicked(const QModelIndex &index);

    void on_comboBox_currentIndexChanged(int index);

private:
    Ui::frmEventView *ui;
    BMS_EventModel *m_evtModel;
    QString m_path="";
    QString m_eventLogPath = "";
    int m_currentPage = -1;
    int m_totalPage = -1;
    int m_pageSize = 10;
};

#endif // FRMEVENTVIEW_H
