#include "historychart.h"

historyChart::historyChart(QWidget *parent)
{
    m_chart = new QChart();

    QLineSeries *s = new QLineSeries();
    m_chart->addSeries(s);

    setChart(m_chart);

    m_dateTimeAxis = new QDateTimeAxis;
    m_dateTimeAxis->setTitleText("DateTime");
    m_dateTimeAxis->setFormat("MM/dd hh:mm:ss");
    m_chart->addAxis(m_dateTimeAxis,Qt::AlignBottom);

    m_voltAxis = new QValueAxis;
    m_voltAxis->setTitleText("Voltage");
    m_voltAxis->setMax(200);
    m_voltAxis->setMin(0);
    m_chart->addAxis(m_voltAxis,Qt::AlignLeft);
    m_ampAxis = new QValueAxis;
    m_ampAxis->setTitleText("Ampere");
    m_ampAxis->setMax(200);
    m_ampAxis->setMin(-200);
    m_chart->addAxis(m_ampAxis,Qt::AlignRight);

    QDateTime dt = QDateTime::currentDateTime();
    for(int i=0;i<100;i++){

        s->append(dt.addSecs(i).toMSecsSinceEpoch(),i*10.);
    }

    s->attachAxis(m_dateTimeAxis);
    s->attachAxis(m_voltAxis);
    QDateTime min = QDateTime::currentDateTime();
    QDateTime max = QDateTime::currentDateTime().addSecs(100);
    m_dateTimeAxis->setRange(min,max);
    //repaint();
}

void historyChart::loadDataFromFile(QString path)
{
    //return;

   // m_chart->removeAllSeries();
    QFile f(path);
    QDateTime minDate, maxDate;
    if(f.open(QIODevice::ReadOnly)){
        QTextStream ts(&f);
        ts.setCodec(QTextCodec::codecForName("Big5"));
        m_chart->removeAllSeries();
        m_series.clear();

        QStringList sl = ts.readLine().split(","); // first line, header
        for(int i=1;i<sl.size();i++){
            QLineSeries *s = new QLineSeries;
            s->setName(sl[i]);
            s->hide();
            m_chart->addSeries(s);
            s->attachAxis(m_dateTimeAxis);
            if(s->name().contains("V")){
                s->attachAxis(m_voltAxis);
            }
            else if(s->name().contains("A")){
                s->attachAxis(m_ampAxis);
            }
            else if(s->name().contains("SOC")){
                s->attachAxis(m_voltAxis);
            }
            else{
                s->attachAxis(m_ampAxis);
            }
            m_series.append(s);
        }

        // load data
        bool minSet = false;
        int i = 0;
        while(!ts.atEnd()){
            sl = ts.readLine().split(",");
            if(!minSet){
                minDate = QDateTime::fromString(sl[0],"yyyy/MM/dd hh:mm:ss");
                minSet = true;
            }
            maxDate = QDateTime::fromString(sl[0],"yyyy/MM/dd hh:mm:ss");
            double stamp = maxDate.toMSecsSinceEpoch();
            //QPointF p = QPointF(maxDate.toMSecsSinceEpoch(),sl[1].toDouble());
            //m_series[0]->append(p);
            m_series[0]->append(stamp,sl[1].toDouble());
            m_series[1]->append(stamp,sl[2].toDouble());
            m_series[2]->append(stamp,sl[3].toDouble());
            m_series[3]->append(stamp,modeMap.value(sl[4]));
            m_series[4]->append(stamp,sl[5].toDouble());
            m_series[5]->append(stamp,sl[6].toDouble());
            m_series[6]->append(stamp,sl[7].toDouble());
            m_series[7]->append(stamp,modeMap.value(sl[8]));
        }

        m_dateTimeAxis->setRange(minDate,minDate.addSecs(3600));
    }

    f.close();
    repaint();
}

void historyChart::showSeries(QStringList names)
{

}

void historyChart::showSeriesByStack(int stack)
{
    //m_chart->removeAllSeries();


//    m_chart->addSeries(m_series[0]);
//    m_series[0]->attachAxis(m_dateTimeAxis);
//    m_series[0]->attachAxis(m_voltAxis);

//        m_series[1]->attachAxis(m_dateTimeAxis);
//        m_series[1]->attachAxis(m_ampAxis);
    foreach (QLineSeries *s, m_series) {
        s->hide();
        if(s->name().contains(QString::number(stack))){
            //m_chart->addSeries(s);
            s->show();
        }

    }

    repaint();
}

void historyChart::setWindowsWidth(int x)
{
    QDateTime dt = m_dateTimeAxis->min();
    m_dateTimeAxis->setRange(dt, dt.addSecs(x));
    repaint();
}

void historyChart::mousePressEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton){
        m_lastPoint = event->pos();
        m_isPress = true;
    }
}

void historyChart::mouseMoveEvent(QMouseEvent *event)
{
    if(!m_coordItem){
        m_coordItem = new QGraphicsSimpleTextItem(this->chart());
        m_coordItem->setZValue(5);
        m_coordItem->setPos(100,60);
        m_coordItem->show();
    }
    const QPoint curPos = event->pos();
    QPointF curVal = this->chart()->mapToValue(QPointF(curPos));

    QString dt = QDateTime::fromMSecsSinceEpoch(curVal.x()).toString("yyyy/MM/dd hh:mm:ss");
    QString coordStr = QString("X = %1, Y = %2").arg(dt).arg(curVal.y());
    m_coordItem->setText(coordStr);

    if(m_isPress){
        QPoint offset = curPos - m_lastPoint;
        m_lastPoint = curPos;

        if(!m_alreadySaveRange){

        }
        this->chart()->scroll(-offset.x(),0);
    }
}

void historyChart::mouseReleaseEvent(QMouseEvent *event)
{
    m_isPress = false;
    if(event->button() == Qt::RightButton){
        if(m_alreadySaveRange){
            this->chart()->axisX()->setRange(m_xMin, m_xMax);
            this->chart()->axisY()->setRange(m_yMin, m_yMax);
        }
    }
}
