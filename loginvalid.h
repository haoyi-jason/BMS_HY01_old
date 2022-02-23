#ifndef LOGINVALID_H
#define LOGINVALID_H

#include <QDialog>

class InputWin;
namespace Ui {
class LoginValid;
}

class LoginValid : public QDialog
{
    Q_OBJECT

public:
    explicit LoginValid(QWidget *parent = nullptr);
    ~LoginValid();
    bool setFileName(QString name);
    int userID();
    void reset();

private slots:
    void on_buttonBox_accepted();
    void on_lineedit_focused(bool state);

    void on_pbOk_clicked();

private:
    Ui::LoginValid *ui;

    QString m_fname;
    InputWin *m_inputWin;
};

#endif // LOGINVALID_H
