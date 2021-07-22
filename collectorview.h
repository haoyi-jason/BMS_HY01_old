#ifndef COLLECTORVIEW_H
#define COLLECTORVIEW_H

#include <QMainWindow>
#include "loginvalid.h"

namespace Ui {
class CollectorView;
}

class frmStackView;
class frmHardwareConfig;
class BMSCollector;
class frmHistoryView;
class frmEventView;
class QTimer;
class QLabel;

class CollectorView : public QMainWindow
{
    Q_OBJECT

public:
    explicit CollectorView(QWidget *parent = nullptr);
    ~CollectorView();

private slots:
    void on_pbStackView_clicked();

    void on_pbHardwareView_clicked();

    void on_pbBatHistory_clicked();

    void on_pbSystemNavi_clicked();

    void on_pbAuth_clicked();

    void on_pbEventView_clicked();

    void on_Issue_Restart_Controller();

    void on_Controller_Offline();

    void on_Idle();

    void auth_accept();
    void auth_reject();

private:
    void hideWindows();

    virtual bool event(QEvent *event) override;


private:
    Ui::CollectorView *ui;

    frmStackView *m_StackWin=nullptr;
    frmHardwareConfig *m_HardwareWin = nullptr;
    frmHistoryView *m_HistWin = nullptr;
    QWidget *mainWidget = nullptr;
    BMSCollector *m_collector = nullptr;
    int m_userID = 1;
    frmEventView *m_evtView = nullptr;
    QTimer *m_idleTimer;
    LoginValid *m_logValid = nullptr;
    QLabel *m_rtcLabel;
    int m_TimeToShutdownScreen = 0;

};

#endif // COLLECTORVIEW_H
