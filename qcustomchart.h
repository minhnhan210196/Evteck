#ifndef QCUSTOMCHART_H
#define QCUSTOMCHART_H

#include <QObject>
#include <QChart>
#include <QValueAxis>
#include <QLineSeries>
#include <QTimer>
#include <hl_simplekalmanfilter.h>

#define MAX_SERIES 1

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
    void replace_series(const QList<QPointF> &points,uint8_t series_index);
    void update_series();
    uint32_t get_max_points();
    void set_filter_en(bool en);
    void set_data_raw_en(bool en);
    float get_max_x();
    float get_min_x();
    float get_max_y();
    float get_min_y();
    void enable_series(uint8_t series_index,bool enable);
    void set_series_name(uint8_t series_index,QString name);
signals:
public slots:
private:
    bool series_enable[MAX_SERIES];
    QLineSeries *series[MAX_SERIES];
    QLineSeries *series_filter[MAX_SERIES];
    QValueAxis *axisX;
    QValueAxis *axisY;
    HL_SimpleKalmanFilter *filter;
    float max_x[MAX_SERIES],max_y[MAX_SERIES],min_x[MAX_SERIES],min_y[MAX_SERIES];
    bool filter_enable,data_raw_enable;
    uint32_t num_display_point;

    QList<float> axisX_val, axisY_val;

    QList<QPointF> series_val;
};

#endif // QCUSTOMCHART_H
