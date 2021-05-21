#ifndef FRMHISTORYVIEW_H
#define FRMHISTORYVIEW_H

#include <QDialog>

namespace Ui {
class frmHistoryView;
}

class frmHistoryView : public QDialog
{
    Q_OBJECT

public:
    explicit frmHistoryView(QWidget *parent = nullptr);
    ~frmHistoryView();

private:
    Ui::frmHistoryView *ui;
};

#endif // FRMHISTORYVIEW_H
