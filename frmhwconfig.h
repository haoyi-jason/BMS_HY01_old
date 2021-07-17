#ifndef FRMHWCONFIG_H
#define FRMHWCONFIG_H

#include <QWidget>

namespace Ui {
class frmHWConfig;
}

class frmHWConfig : public QWidget
{
    Q_OBJECT

public:
    explicit frmHWConfig(QWidget *parent = nullptr);
    ~frmHWConfig();

private:
    Ui::frmHWConfig *ui;
};

#endif // FRMHWCONFIG_H
