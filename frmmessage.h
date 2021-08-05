#ifndef FRMMESSAGE_H
#define FRMMESSAGE_H

#include <QWidget>

namespace Ui {
class frmMessage;
}

class frmMessage : public QWidget
{
    Q_OBJECT

public:
    explicit frmMessage(QWidget *parent = nullptr);
    ~frmMessage();

    void setText(QString title, QString content);

signals:
    void ok();
    void cancel();

private slots:
    void on_pbOK_clicked();

    void on_pbCancel_clicked();

private:
    Ui::frmMessage *ui;
};

#endif // FRMMESSAGE_H
