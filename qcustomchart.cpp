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
    this->axisX->setRange(this->min_x,this->max_y);
    this->axisY->setRange(this->min_y,this->max_y);

    this->addSeries(this->series);

    this->addAxis(this->axisX,Qt::AlignBottom);
    this->addAxis(this->axisY,Qt::AlignLeft);

    this->series->attachAxis(this->axisX);
    this->series->attachAxis(this->axisY);

    this->legend()->setMarkerShape(QLegend::MarkerShapeCircle);

    this->time_update = new QTimer();

    connect(this->time_update,SIGNAL(timeout()),this,SLOT(update_series()));

    this->time_update->start(10);

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
void QCustomChart::update_series()
{

    static uint32_t range = 0;

    if(this->axisY_val.count() >= this->num_display_point){
        range += this->num_display_point;
        for(uint32_t i = 0;i<this->num_display_point;i++){
            uint32_t x = range - this->num_display_point +i;
            this->axisX_val.append(x);
            QPointF point(x,this->axisY_val.at(i));
            this->series_val.append(point);

        }
        this->min_x = *std::min_element(this->axisX_val.begin(),this->axisX_val.end());
        this->max_x = *std::max_element(this->axisX_val.begin(),this->axisX_val.end());
        this->min_y = *std::min_element(this->axisY_val.begin(),this->axisY_val.end());
        this->max_y = *std::max_element(this->axisY_val.begin(),this->axisY_val.end());

        this->axisX->setRange(this->min_x,this->max_x);
        this->axisY->setRange(this->min_y,this->max_y);

        this->series->replace(this->series_val);

        this->axisX_val.clear();
        this->axisY_val.clear();
        this->series_val.clear();
    }
}
