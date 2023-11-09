#include "qcustomchart.h"
#include <stdio.h>
#include <stdint.h>

using namespace std;

#define MAX_DISPLAY_POINT 1000

QCustomChart::QCustomChart(QChart *parent)
    : QChart{parent}
{
    this->num_display_point = MAX_DISPLAY_POINT;
    for(uint8_t i = 0;i<MAX_SERIES;i++){
        this->max_x[i] = this->num_display_point;
        this->min_x[i] = 0;
        this->max_y[i] = 1;
        this->min_y[i] = 0;
        this->series[i] = new QLineSeries();
        this->series_filter[i] = new QLineSeries();
        this->addSeries(this->series[i]);
        this->addSeries(this->series_filter[i]);
    }
    this->data_raw_enable = true;
    this->filter = new HL_SimpleKalmanFilter(1,1.0,1);

    this->axisX = new QValueAxis();
    this->axisY = new QValueAxis();
    this->axisX->setRange(this->min_x[0],this->max_x[0]);
    this->axisY->setRange(this->min_y[0],this->max_y[0]);
    this->filter_enable = false;



    this->addAxis(this->axisX,Qt::AlignBottom);
    this->addAxis(this->axisY,Qt::AlignLeft);
    for(uint8_t i = 0;i<MAX_SERIES;i++){
        this->series[i]->attachAxis(this->axisX);
        this->series[i]->attachAxis(this->axisY);
        this->series_filter[i]->attachAxis(this->axisX);
        this->series_filter[i]->attachAxis(this->axisY);
        this->series[i]->setName("data");
        this->series_filter[i]->setName("data filter");
        this->series[i]->setUseOpenGL(true);
        this->series_filter[i]->setUseOpenGL(true);
    }
    this->axisX->setTitleText("time(ms)");
    this->axisY->setTitleText("value");
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

void QCustomChart::replace_series(const QList<QPointF> &points,uint8_t series_index)
{
    if(series_index >= MAX_SERIES) return;
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
    this->min_x[series_index] = *std::min_element(_x.begin(),_x.end());
    this->max_x[series_index] = *std::max_element(_x.begin(),_x.end());

    float _min_y = *std::min_element(_y.begin(),_y.end());
    float _max_y = *std::max_element(_y.begin(),_y.end());
    if(_min_y < this->min_y[series_index]) this->min_y[series_index] = _min_y;
    if(_max_y > this->max_y[series_index]) this->max_y[series_index] = _max_y;



    this->axisX->setRange(this->min_x[series_index],this->max_x[series_index]);
    this->axisY->setRange(_min_y-1,_max_y+1);
    if(filter_enable){
        this->series_filter[series_index]->replace(*data_filter);
    }
    if(this->data_raw_enable == true){
        this->series[series_index]->replace(points);
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
//        this->min_x = *std::min_element(_x.begin(),_x.end());
//        this->max_x = *std::max_element(_x.begin(),_x.end());
//        this->min_y = *std::min_element(_y.begin(),_y.end());
//        this->max_y = *std::max_element(_y.begin(),_y.end());
//        this->axisX->setRange(this->min_x,this->max_x);
//        this->axisY->setRange(this->min_y-1,this->max_y+1);
//        this->series->replace(this->series_val);
//        this->series->clear();
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
   float max = 0;
   for(uint8_t i = 0;i<MAX_SERIES;i++){
        if(this->series_enable[i] == false) continue;
        if(this->max_x[i] > max)
            max = this->max_x[i];
   }
   return max;
}

float QCustomChart::get_min_x()
{
    float max = 0;
    for(uint8_t i = 0;i<MAX_SERIES;i++){
        if(this->series_enable[i] == false) continue;
         if(this->min_x[i] < max)
             max = this->min_x[i];
    }
    return max;
}

float QCustomChart::get_max_y()
{
    float max = 0;
    for(uint8_t i = 0;i<MAX_SERIES;i++){
        if(this->series_enable[i] == false) continue;
         if(this->max_y[i] > max)
             max = this->max_y[i];
    }
    return max;
}

float QCustomChart::get_min_y()
{
    float max = 0;
    for(uint8_t i = 0;i<MAX_SERIES;i++){
        if(this->series_enable[i] == false) continue;
        if(this->min_y[i] < max)
             max = this->min_y[i];
    }
    return max;
}

void QCustomChart::enable_series(uint8_t series_index, bool enable)
{
    this->series_enable[series_index] = enable;
    this->series_filter[series_index]->setVisible(enable);
    this->series[series_index]->setVisible(enable);
}

void QCustomChart::set_series_name(uint8_t series_index, QString name)
{
    this->series[series_index]->setName(name);
    this->series_filter[series_index]->setName(name+"_filter");
}

