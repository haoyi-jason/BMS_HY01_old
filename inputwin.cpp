#include "inputwin.h"
#include <QGridLayout>
#include <QLabel>

InputWin::InputWin(QWidget *parent) : QDialog(parent)
{
    //this->resize(320,240);

    inBox1 = new QLineEdit();
    inBox1->setReadOnly(true);
    inBox1->setAlignment(Qt::AlignRight);
    inBox1->setMaxLength(16);

    QFont f = inBox1->font();
    f.setPointSize(f.pointSize()+16);
    f.setBold(true);
    inBox1->setFont(f);

    for(int i=0;i<10;i++){
        Button *btn =new Button(QString::number(i));
        connect(btn,&Button::clicked,this,&InputWin::padClicked);
        m_buttons.append(btn);
    }

    Button *dot = new Button(".");
    connect(dot,&Button::clicked,this,&InputWin::padClicked);
    Button *ok = new Button("OK");
    connect(ok,&Button::clicked,this,&InputWin::padClicked);
    Button *can = new Button("CAN");
    connect(can,&Button::clicked,this,&InputWin::padClicked);
//    Button *clr = new Button("CLR");
    Button *clr = new Button("+/-");
    connect(clr,&Button::clicked,this,&InputWin::padClicked);
    Button *bck = new Button("<-");
    connect(bck,&Button::clicked,this,&InputWin::padClicked);

    hintLabe = new QLabel("MIN:0, MAX:9999");


    QGridLayout *ml = new QGridLayout;
    ml->setSizeConstraint(QLayout::SetFixedSize);
    ml->addWidget(inBox1,0,0,1,3);
    ml->addWidget(hintLabe,1,0,1,3);
    ml->addWidget(bck,2,0);
    ml->addWidget(clr,2,1);
    ml->addWidget(can,2,2);

    for(int i=1;i<m_buttons.size();i++){
        int r = ((9-i)/3)+3;
        int c = ((i-1)%3);
        ml->addWidget(m_buttons[i],r,c);
    }
    ml->addWidget(m_buttons[0],6,0);
    ml->addWidget(dot,6,1);
    ml->addWidget(ok,6,2);

    setLayout(ml);
    //setWindowFlags(Qt::FramelessWindowHint|Qt::NoDropShadowWindowHint| Qt::Window);
    setWindowModality(Qt::ApplicationModal);
    setWindowTitle("Input");
}

void InputWin::padClicked()
{
    Button *btn = (Button*)sender();
    QString str = inBox1->text();
    QString key = btn->text();
    switch(key_map.value(key)){
    case 0:case 1:case 2:case 3:case 4:case 5:case 6:case 7:case 8:case 9:
        if(inBox1->selectedText().size()){
            str = btn->text();
        }
        else{
            str += btn->text();
        }
        if(m_isNumbering){
            int v = str.toInt();
            if((v > m_maxInput) || (v < m_minInput)){
                inBox1->setText(str);
            }
        }
        else{
            inBox1->setText(str);
        }
        break;
    case 10: // dot
        str += btn->text();
        break;
    case 11: // OK
        if(m_checkInput){
            int v = inBox1->text().toInt();
            if((v <= m_maxInput) && (v >= m_minInput)){
                emit result(inBox1->text());
                emit QDialog::accept();
            }
            else{

            }
        }
        else{
            emit result(inBox1->text());
            emit QDialog::accept();
        }
        close();
        //hide();
        break;
    case 12: // cancel
        //hide();
        emit QDialog::reject();
        close();
        break;
    case 13: // back

        str = str.left(str.size()-1);
        break;
    case 14: // clear
    {
       if(str.left(1)=="-"){
           str = str.right(str.size()-1);
       }
       else{
           str = "-"+str;
       }
    }
     //   str = "";
        break;
    default:break;
    }
    if(m_passChar){
        int n = str.length();
        QString b("*");
        b.repeated(n);
        inBox1->setText(b);

    }
    else{
        inBox1->setText(str);
    }
}

void InputWin::setDisplayContent(QString str)
{
    inBox1->setText(str);
    inBox1->selectAll();
}

void InputWin::setLimit(int max, int min)
{
    m_minInput = min;
    m_maxInput = max;
}

void InputWin::password(bool enable)
{
    m_passChar = enable;
}

void InputWin::checkInput(bool enable)
{
    m_checkInput = enable;
}
