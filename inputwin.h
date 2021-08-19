#ifndef INPUTWIN_H
#define INPUTWIN_H

#include <QDialog>
#include <QToolButton>
#include <QLineEdit>
#include <QLabel>
#include <QEvent>

class Button:public QToolButton{
    Q_OBJECT
public:
    explicit Button(const QString &text,QWidget *parent = nullptr):QToolButton(parent)
    {
        setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Preferred);
        setText(text);
    }

    QSize sizeHint() const override{
        QSize size = QToolButton::sizeHint();
        size.rheight() += 20;
        size.rwidth() = qMax(size.width(),size.height());
        return size;
    }
};

class FocusEditor:public QLineEdit
{
    Q_OBJECT
public:
    explicit FocusEditor(QWidget *parent = nullptr):QLineEdit(parent)
    {
        setAlignment(Qt::AlignRight);
        m_minVal = 0;
        m_maxVal = 0;
    }

    void setRange(int min, int max){m_minVal = min;m_maxVal = max;}
    int min(){return m_minVal;}
    int max(){return m_maxVal;}
    void passWordMode(bool b){setEchoMode(b?QLineEdit::Password:QLineEdit::Normal);}

signals:
    void focused(bool b);
public slots:
    void receiveChange(QString msg){setText(msg);}

protected:
    virtual void focusInEvent(QFocusEvent *e){
        QLineEdit::focusInEvent(e);
    }
    virtual void focusOutEvent(QFocusEvent *e){
        QLineEdit::focusOutEvent(e);
    }
    virtual bool event(QEvent *e){
        QLineEdit::event(e);
        if(e->type() == QEvent::MouseButtonRelease){
            emit focused(true);
        }
        return true;
    }

private:
    int m_minVal = 0;
    int m_maxVal = 9999;
};

static QMap<QString,int> key_map{
    {"0",0},
    {"1",1},
    {"2",2},
    {"3",3},
    {"4",4},
    {"5",5},
    {"6",6},
    {"7",7},
    {"8",8},
    {"9",9},
    {".",10},
    {"OK",11},
    {"CAN",12},
    {"<-",13},
    {"+/-",14},
};

class InputWin : public QDialog
{
    Q_OBJECT
public:
    explicit InputWin(QWidget *parent = nullptr);

private:
    Button *createButton(const QString &text, const char *member);

signals:
    void result(QString);
    void isValie(bool);

public slots:
    void setDisplayContent(QString str);
    void setLimit(int max = INT_MAX, int min=INT_MIN);
    void password(bool enable);
    void checkInput(bool enable);

private slots:
    void padClicked();


private:
    bool m_checkInput = false;
    bool m_passChar = false;
    bool m_isNumbering = true;
    int m_maxInput = INT_MAX;
    int m_minInput = INT_MIN;
    QString m_inputString;
    QLineEdit *inBox1;
    QList<Button*> m_buttons;
    QLabel *hintLabe;
    QString m_validString="";

};

#endif // INPUTWIN_H
