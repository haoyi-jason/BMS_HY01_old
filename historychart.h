#ifndef HISTORYCHARG_H
#define HISTORYCHARG_H
#include <QChartView>
#include <QtCharts>


using namespace QtCharts;

class historyChart : public QChartView
{

public:
    const QMap<QString, int> modeMap = {{"放電",-1},{"閒置",0},{"充電",1}};
    historyChart(QWidget *parent = nullptr);
    void loadDataFromFile(QString path);
    void showSeries(QStringList names);
    void showSeriesByStack(int stack);
    void setWindowsWidth(int x);

protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);

private:
    QChart *m_chart;
    QList<QLineSeries*> m_series;
    QDateTimeAxis *m_dateTimeAxis;
    QValueAxis *m_voltAxis;
    QValueAxis *m_ampAxis;

    bool m_isPress = false;
    bool m_ctrlPress;
    QPoint m_lastPoint;
    QGraphicsSimpleTextItem *m_coordItem=nullptr;
    bool m_alreadySaveRange = false;

    double m_xMin, m_xMax, m_yMin, m_yMax;

};

#endif // HISTORYCHARG_H
