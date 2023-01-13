#include "chart.h"

Chart::Chart()
{
    this->max_x = 10;
    this->max_y = 10;
    this->min_x = 0;
    this->min_y = 0;
    this->serial = new QLineSeries();
    this->chart = new QChart();
    this->chart->addSeries(this->serial);
    this->chart->legend()->hide();
    axisX = new QValueAxis;
    axisX->setTickCount(10);
    axisX->setRange(this->min_x,this->max_x);
    axisX->setLabelFormat("%f");
    this->chart->addAxis(axisX, Qt::AlignBottom);
    this->serial->attachAxis(axisX);
    axisY = new QValueAxis;
    axisY->setLabelFormat("%f");
    axisY->setRange(this->min_y,this->max_y);
    this->chart->addAxis(axisY, Qt::AlignLeft);
    this->serial->attachAxis(axisY);
    this->chart->setAnimationDuration(10);
    this->chart->setTheme(QChart::ChartThemeBlueCerulean);
    this->view = new QChartView(this->chart);
    this->view->setMinimumWidth(480);
    this->view->setMinimumHeight(320);
}

Chart::~Chart()
{
    delete this->chart;
    delete this->serial;
    delete this->view;
}

void Chart::append(qreal x, qreal y)
{
    this->serial->append(x,y);
}

QChart *Chart::get_chart()
{
    return this->chart;
}

QChartView *Chart::get_chart_view()
{
    return this->view;
}

void Chart::update()
{
    this->chart->update();
}
