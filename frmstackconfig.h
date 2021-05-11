#ifndef FRMSTACKCONFIG_H
#define FRMSTACKCONFIG_H

#include <QWidget>

namespace Ui {
class frmStackConfig;
}

class frmStackConfig : public QWidget
{
    Q_OBJECT

public:
    explicit frmStackConfig(QWidget *parent = nullptr);
    ~frmStackConfig();

private:
    Ui::frmStackConfig *ui;
};

#endif // FRMSTACKCONFIG_H
