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
    void setLogFile(QString path);

private slots:
    void on_pbClearAll_clicked();
    void dummy();

private:
    Ui::frmEventView *ui;
    BMS_EventModel *m_evtModel;
    QString m_path="";
};

#endif // FRMEVENTVIEW_H
