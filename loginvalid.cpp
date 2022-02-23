#include "loginvalid.h"
#include "ui_loginvalid.h"
#include "inputwin.h"
#include<QJsonDocument>
#include<QJsonObject>
#include<QJsonValue>
#include<QJsonArray>
#include <QFile>
#include <QMessageBox>
#include <QCryptographicHash>
#include "inputwin.h"

LoginValid::LoginValid(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LoginValid)
{
    ui->setupUi(this);
    ui->lePasswd_2->setVisible(false);

   // setModal(true);
    if(!QSysInfo().productType().contains("win")){

        setWindowFlags(Qt::WindowStaysOnTopHint);
        //setWindowFlags();
        //showFullScreen();
    }


    //
    connect(ui->lePasswd,&FocusEditor::focused,this,&LoginValid::on_lineedit_focused);
    connect(ui->lePasswd_2,&FocusEditor::focused,this,&LoginValid::on_lineedit_focused);

    //connect(ui->lePasswd,&FocusEditor::,this,&LoginValid::on_lineedit_focused);

    m_inputWin = new InputWin(this);

}

LoginValid::~LoginValid()
{
    delete ui;
}

bool LoginValid::setFileName(QString name)
{
    if(name.isEmpty()) return false;
    m_fname = name;
    QFile f(m_fname);
    if(f.open(QIODevice::ReadOnly)){
        QJsonParseError e;
        QJsonDocument d = QJsonDocument::fromJson(f.readAll(),&e);
        f.close();
        if(d.isNull()){
            return false;
        }
        else{
            return true;
        }
    }
    return false;
}
void LoginValid::on_buttonBox_accepted()
{


}

void LoginValid::on_lineedit_focused(bool state)
{
    //qDebug()<<Q_FUNC_INFO;
    FocusEditor *editor = (FocusEditor*)sender();
    //InputWin *w = new InputWin(this);
    //w->setAttribute(Qt::WA_DeleteOnClose);
    //w->setDisplayContent(editor->text());
    //connect(w,&InputWin::result,editor,&FocusEditor::setText);
    //w->exec();
//    m_inputWin = new InputWin(this);
//    m_inputWin->setAttribute(Qt::WA_DeleteOnClose);
    m_inputWin->setDisplayContent(editor->text());
    connect(m_inputWin,&InputWin::result,editor,&FocusEditor::setText);
    //m_inputWin->exec();
    //m_inputWin->setModal(true);
    m_inputWin->hide();
    m_inputWin->show();
}

void LoginValid::on_pbOk_clicked()
{
    QFile f(m_fname);
    if(f.open(QIODevice::ReadOnly)){
        QJsonParseError e;
        QJsonDocument d = QJsonDocument::fromJson(f.readAll(),&e);
        f.close();
        QJsonObject obj = d.object();
        if(obj.contains("account")){
            QJsonArray arr = obj["account"].toArray();
            QJsonObject o;
            QString t = ui->comboBox->currentIndex()==0?"user":"admin";
            bool found = false;
            int index = 0;
            for(auto v:arr){
                o = v.toObject();
                if(o["name"].toString().trimmed() == t){
                    found = true;
                    break;
                }
                index++;
            }

            if(found){
                if(o["passwd"].toString().trimmed().isEmpty()){
                    if(ui->lePasswd_2->isVisible()){
                        if(ui->lePasswd->text() == ui->lePasswd_2->text()){
                            QString p=QString(QCryptographicHash::hash(ui->lePasswd->text().toUtf8(),QCryptographicHash::Sha256));
                            o["passwd"] = p;
                            ui->lePasswd_2->setVisible(false);
                            arr[index] = o;
                            obj["account"] = arr;
                            d.setObject(obj);
                            if(f.open(QIODevice::ReadWrite)){
                                f.write(d.toJson());
                                f.close();
                            }
                            accept();
                        }
                        else{
                            QMessageBox::warning(this,"錯誤!","密碼不符合");
                        }
                    }
                    else{
                        QMessageBox::warning(this,"訊息","第一次使用本帳號,請設定密碼!");
                        ui->lePasswd_2->setVisible(true);
                    }
                }
                else{
                    QString a,b;
                    a = o["passwd"].toString();
                    b = QString(QCryptographicHash::hash(ui->lePasswd->text().toUtf8(),QCryptographicHash::Sha256));
                    if(a==b){
//                    if(o["passwd"].toString().trimmed() == ui->lePasswd->text().trimmed()){
                        accept();
                    }
                    else
                    {
                        QMessageBox::warning(this,"錯誤","密碼輸入錯誤");
                    }
                }
            }
        }
    }
    else{
        reject();
    }
}

int LoginValid::userID()
{
    return ui->comboBox->currentIndex();
}

void LoginValid::reset()
{
    ui->lePasswd->setText("");
    ui->lePasswd_2->setText("");
}
