#include "qcustomchart.h"
#include <stdio.h>
#include <stdint.h>

using namespace std;

#define MAX_DISPLAY_POINT 1000

QCustomChart::QCustomChart(QChart *parent)
    : QChart{parent}
{
    this->num_display_point = MAX_DISPLAY_POINT;
    this->max_x = this->num_display_point;
    this->min_x = 0;
    this->max_y = 1;
    this->min_y = 0;
    this->data_raw_enable = true;
    this->filter = new HL_SimpleKalmanFilter(1,1.0,1);
    this->series = new QLineSeries();
    this->series_filter = new QLineSeries();
    this->axisX = new QValueAxis();
    this->axisY = new QValueAxis();
    this->axisX->setRange(this->min_x,this->max_x);
    this->axisY->setRange(this->min_y,this->max_y);
    this->filter_enable = false;
    this->addSeries(this->series);
    this->addSeries(this->series_filter);


    this->addAxis(this->axisX,Qt::AlignBottom);
    this->addAxis(this->axisY,Qt::AlignLeft);

    this->series->attachAxis(this->axisX);
    this->series->attachAxis(this->axisY);
    this->series_filter->attachAxis(this->axisX);
    this->series_filter->attachAxis(this->axisY);
    this->series_filter->setColor(QColor("red"));
    this->axisX->setTitleText("time(ms)");
    this->axisY->setTitleText("value");

    this->series->setName("data");
    this->series_filter->setName("data filter");
    this->series->setUseOpenGL(true);
    this->series_filter->setUseOpenGL(true);
    this->legend()->setMarkerShape(QLegend::MarkerShapeCircle);
}

QCustomChart::~QCustomChart()
{

}

void QCustomChart::add_PointY(QPointF point)
{
    this->series_val.append(point);
}

void QCustomChart::set_max_point(uint32_t max)
{
    this->num_display_point = max;
    //this->axisX->setRange(0,this->num_display_point);
}

void QCustomChart::set_axisX_title(QString title)
{
    this->axisX->setTitleText(title);
}

void QCustomChart::set_axisY_title(QString title)
{
    this->axisY->setTitleText(title);
}

void QCustomChart::replace_series(const QList<QPointF> &points)
{
    QList<float> _x;
    QList<float> _y;
    QList<QPointF> *data_filter = new QList<QPointF>;
    for(qsizetype i = 0;i<points.count();i++){
        _x.append(points.at(i).x());
        _y.append(points.at(i).y());
        if(filter_enable){
            data_filter->append(QPointF(points.at(i).x(),this->filter->update(points.at(i).y())));
        }
    }
    this->min_x = *std::min_element(_x.begin(),_x.end());
    this->max_x = *std::max_element(_x.begin(),_x.end());
    this->min_y = *std::min_element(_y.begin(),_y.end());
    this->max_y = *std::max_element(_y.begin(),_y.end());
    this->axisX->setRange(this->min_x,this->max_x);
    this->axisY->setRange(this->min_y-1,this->max_y+1);
    if(filter_enable){
        this->series_filter->replace(*data_filter);
    }
    if(this->data_raw_enable == true){
        this->series->replace(points);
    }
    data_filter->clear();
    delete data_filter;
}

void QCustomChart::update_series()
{
    if(this->series_val.count() > this->num_display_point){
        QList<float> _x;
        QList<float> _y;
        for(qsizetype i = 0;i<this->series_val.count();i++){
            _x.append(this->series_val.at(i).x());
            _y.append(this->series_val.at(i).y());
        }
        this->min_x = *std::min_element(_x.begin(),_x.end());
        this->max_x = *std::max_element(_x.begin(),_x.end());
        this->min_y = *std::min_element(_y.begin(),_y.end());
        this->max_y = *std::max_element(_y.begin(),_y.end());
        this->axisX->setRange(this->min_x,this->max_x);
        this->axisY->setRange(this->min_y-1,this->max_y+1);
        this->series->replace(this->series_val);
        this->series->clear();
    }
}

uint32_t QCustomChart::get_max_points()
{
    return this->num_display_point;
}

void QCustomChart::set_filter_en(bool en)
{
    this->filter_enable = en;
}

void QCustomChart::set_data_raw_en(bool en)
{
    this->data_raw_enable = en;
}

float QCustomChart::get_max_x()
{
   return this->max_x;
}

float QCustomChart::get_min_x()
{
    return this->min_x;
}

float QCustomChart::get_max_y()
{
    return this->max_y;
}

float QCustomChart::get_min_y()
{
    return this->min_y;
}

