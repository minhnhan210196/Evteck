#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtCharts/QLineSeries>
#include <QtCharts/QChartView>
#include <QtCharts/QValueAxis>
#include <QRandomGenerator>
#include <QFile>
#include <QList>
#include <evteck_chart.h>
#include <chart.h>
#include "QTimer"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

#define NUM_CHANEL 4

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    void chart_update();
private slots:
    void on_BloodPressureTab_currentChanged(int index);

    void on_open_button_clicked();

    void on_start_draw_chart_clicked();

    void on_stop_draw_char_clicked();

private:

    void creat_chart();
    Ui::MainWindow *ui;
    Chart *heart_beat_chart;
    Chart *bool_chart;
    Chart *pwv_chart;
    Evteck_Chart *evteck_chart;
    int m_listCount;
    int m_valueMax;
    int m_valueCount;
    QString file_name;
    QFile file;
    QTextStream stream_in;
    QTimer *timer;
    QList<QString> data_read_file;
    QList<float> file_time;
    QList<float> data_sensor[NUM_CHANEL];
    QList<float> fdata_sensor[NUM_CHANEL];
    QStringList sensor_list;
};
#endif // MAINWINDOW_H
