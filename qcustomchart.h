#ifndef QCUSTOMCHART_H
#define QCUSTOMCHART_H

#include <QObject>
#include <QChart>
#include <QValueAxis>
#include <QLineSeries>
#include <QTimer>
#include <hl_simplekalmanfilter.h>


class QCustomChart : public QChart
{
    Q_OBJECT
public:
    explicit QCustomChart(QChart *parent = nullptr);
    ~QCustomChart();
    void add_PointY(QPointF point);
    void set_max_point(uint32_t max);
    void set_axisX_title(QString title);
    void set_axisY_title(QString title);
    void replace_series(const QList<QPointF> &points);
    void update_series();
    uint32_t get_max_points();
    void set_filter_en(bool en);
    void set_data_raw_en(bool en);
    float get_max_x();
    float get_min_x();
    float get_max_y();
    float get_min_y();
signals:
public slots:
private:
    QLineSeries *series;
    QLineSeries *series_filter;
    QValueAxis *axisX;
    QValueAxis *axisY;
    HL_SimpleKalmanFilter *filter;
    float max_x,max_y,min_x,min_y;
    bool filter_enable,data_raw_enable;
    uint32_t num_display_point;

    QList<float> axisX_val, axisY_val;

    QList<QPointF> series_val;
};

#endif // QCUSTOMCHART_H
