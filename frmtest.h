#ifndef FRMTEST_H
#define FRMTEST_H

#include <QWidget>

namespace Ui {
class frmTest;
}

class frmTest : public QWidget
{
    Q_OBJECT

public:
    explicit frmTest(QWidget *parent = nullptr);
    ~frmTest();

private:
    Ui::frmTest *ui;
};

#endif // FRMTEST_H
