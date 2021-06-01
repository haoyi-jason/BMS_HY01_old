#ifndef COLLECTORVIEW_H
#define COLLECTORVIEW_H

#include <QMainWindow>

namespace Ui {
class CollectorView;
}

class frmStackView;
class frmHardwareConfig;
class BMSCollector;
class frmHistoryView;

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

private:
    void hideWindows();

private:
    Ui::CollectorView *ui;

    frmStackView *m_StackWin=nullptr;
    frmHardwareConfig *m_HardwareWin = nullptr;
    frmHistoryView *m_HistWin = nullptr;
    QWidget *mainWidget = nullptr;
    BMSCollector *m_collector = nullptr;
};

#endif // COLLECTORVIEW_H
