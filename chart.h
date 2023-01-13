#ifndef CHART_H
#define CHART_H

#include "QChart"
#include "QChartView"
#include "QLineSeries"
#include <QApplication>
#include <QList>
#include <QValueAxis>

class Chart
{
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
public:
    Chart();
    ~Chart();
    void append(qreal x,qreal y);
    QChart *get_chart();
    QChartView *get_chart_view();
    void update();
};

#endif // CHART_H
