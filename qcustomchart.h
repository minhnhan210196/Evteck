#ifndef QCUSTOMCHART_H
#define QCUSTOMCHART_H

#include <QObject>
#include <QChart>
#include <QValueAxis>
#include <QLineSeries>
#include <QTimer>



class QCustomChart : public QChart
{
    Q_OBJECT
public:
    explicit QCustomChart(QChart *parent = nullptr);
    ~QCustomChart();
    void add_PointY(float y);
    void set_max_point(uint32_t max);
    void set_axisX_title(QString title);
    void set_axisY_title(QString title);
    void replace_series(const QList<QPointF> &points);
    uint32_t get_max_points();
signals:
public slots:
private:
    QLineSeries *series;
    QValueAxis *axisX;
    QValueAxis *axisY;

    float max_x,max_y,min_x,min_y;

    uint32_t num_display_point;

    QList<float> axisX_val, axisY_val;

    QList<QPointF> series_val;
};

#endif // QCUSTOMCHART_H
