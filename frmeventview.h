#ifndef FRMEVENTVIEW_H
#define FRMEVENTVIEW_H

#include <QDialog>

namespace Ui {
class frmEventView;
}

class frmEventView : public QDialog
{
    Q_OBJECT

public:
    explicit frmEventView(QWidget *parent = nullptr);
    ~frmEventView();

private:
    Ui::frmEventView *ui;
};

#endif // FRMEVENTVIEW_H
