#include "qcustomchart.h"


#define MAX_DISPLAY_POINT 1000

QCustomChart::QCustomChart(QChart *parent)
    : QChart{parent}
{
    this->num_display_point = MAX_DISPLAY_POINT;
    this->max_x = this->num_display_point;
    this->min_x = 0;
    this->max_y = 1;
    this->min_y = 0;

    this->series = new QLineSeries();
    this->axisX = new QValueAxis();
    this->axisY = new QValueAxis();
    this->axisX->setRange(this->min_x,this->max_x);
    this->axisY->setRange(this->min_y,this->max_y);

    this->addSeries(this->series);
    this->addAxis(this->axisX,Qt::AlignBottom);
    this->addAxis(this->axisY,Qt::AlignLeft);

    this->series->attachAxis(this->axisX);
    this->series->attachAxis(this->axisY);

    this->legend()->setMarkerShape(QLegend::MarkerShapeCircle);
}

QCustomChart::~QCustomChart()
{

}

void QCustomChart::add_PointY(float y)
{
    this->axisY_val.append(y);
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
    for(uint64_t i = 0;i<points.count();i++){
        _x.append(points.at(i).x());
        _y.append(points.at(i).y());
    }

    this->min_x = *std::min_element(_x.begin(),_x.end());
    this->max_x = *std::max_element(_x.begin(),_x.end());
    this->min_y = *std::min_element(_y.begin(),_y.end());
    this->max_y = *std::max_element(_y.begin(),_y.end());
    this->axisX->setRange(this->min_x,this->max_x);
    this->axisY->setRange(this->min_y-1,this->max_y+1);
    this->series->replace(points);
}

uint32_t QCustomChart::get_max_points()
{
    return this->num_display_point;
}

