#ifndef FRMHARDWARECONFIG_H
#define FRMHARDWARECONFIG_H

#include <QWidget>

namespace Ui {
class frmHardwareConfig;
}

class frmHardwareConfig : public QWidget
{
    Q_OBJECT

public:
    explicit frmHardwareConfig(QWidget *parent = nullptr);
    ~frmHardwareConfig();

private:
    Ui::frmHardwareConfig *ui;
};

#endif // FRMHARDWARECONFIG_H
