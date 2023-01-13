#ifndef EVTECK_CHART_H
#define EVTECK_CHART_H

#include "QChart"
#include "QChartView"
#include "QLineSeries"
#include <QApplication>
#include <QList>
#include <QValueAxis>

class Evteck_Chart
{
public:
    Evteck_Chart();
    ~Evteck_Chart();
    void append(qreal x,qreal y);
    QChart *get_chart();
    QChartView *get_chart_view();
    void update();
    void setX_Axis(qreal min,qreal max);
    void setY_Axis(qreal min,qreal max);
private:
    double max_x;
    double max_y;
    double min_x;
    double min_y;
    QChart *chart;
    QChartView *view;
    QLineSeries *serial;
    QValueAxis *axisX;
    QValueAxis *axisY;

};

#endif // EVTECK_CHART_H
