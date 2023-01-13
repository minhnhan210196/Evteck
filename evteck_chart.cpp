#include "evteck_chart.h"

Evteck_Chart::Evteck_Chart()
{
    this->min_x = 0;
    this->max_x = 1000;

    this->min_y = -0.001;
    this->max_y = 0.001;
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
    this->chart->setTheme(QChart::ChartThemeBlueCerulean);
    this->view = new QChartView(this->chart);
    this->view->setMinimumWidth(480);
    this->view->setMinimumHeight(320);

}

Evteck_Chart::~Evteck_Chart()
{
    delete this->chart;
    delete this->serial;
    delete this->view;
}

void Evteck_Chart::append(qreal x, qreal y)
{

    if(this->min_x > x) this->min_x = x;
    else if(this->max_x < x) this->max_x = x;
    if(this->min_y > y) this->min_y = y;
    else if(this->max_y < y) this->max_y = y;

    this->axisX->setRange(min_x,max_x);
    this->axisY->setRange(min_y,max_y);

    this->serial->append(x,y);
}

QChart *Evteck_Chart::get_chart()
{
    return this->chart;
}

QChartView *Evteck_Chart::get_chart_view()
{
    return this->view;
}

void Evteck_Chart::update()
{
    this->chart->update();
}

void Evteck_Chart::setX_Axis(qreal min, qreal max)
{
    this->axisX->setRange(min,max);
}

void Evteck_Chart::setY_Axis(qreal min, qreal max)
{
    this->axisY->setRange(min,max);
}
