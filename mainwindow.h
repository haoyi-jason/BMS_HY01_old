#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class StackInfo;
class frmHardwareConfig;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_pbSystemMonitor_clicked();

    void on_pbHardwareInfo_clicked();

    void on_pbSysConfig_clicked();

    void on_pbViewEvent_clicked();

    void on_pbSystemMonitor_2_clicked();

private:
    Ui::MainWindow *ui;
    StackInfo *m_stackWin;
    frmHardwareConfig *m_hardwareConfig;
};

#endif // MAINWINDOW_H
