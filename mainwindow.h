#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtCharts/QLineSeries>
#include <QtCharts/QChartView>
#include <QtCharts/QValueAxis>
#include <QtCharts/QSplineSeries>
#include <QRandomGenerator>
#include <QFile>
#include <QList>
#include <chart.h>
#include <chartview.h>
#include "QTimer"
#include "setting.h"
#include "QTcpSocket"
#include "save_data.h"
#include "QTime"
#include <QQueue>
#include <qcustomchart.h>
#include <QElapsedTimer>

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
    void connected();
    void disconnected();
    void bytesWritten(qint64 bytes);
    void readyRead();
    void update_senor_slot();
    void gen_data_sensor();
    void fps_run_time();
private slots:
    void on_BloodPressureTab_currentChanged(int index);

    void on_open_button_clicked();

    void on_start_draw_chart_clicked();

    void on_stop_draw_char_clicked();

    void on_clear_button_clicked();


    void on_s1_raw_checkbox_stateChanged(int arg1);

    void on_s1_filter_checkbox_stateChanged(int arg1);

    void on_bandpass_checkbox_stateChanged(int arg1);

    void on_bandstop_checkbox_stateChanged(int arg1);

    void on_kalman_checkbox_stateChanged(int arg1);

    void on_v_to_h_checkbox_stateChanged(int arg1);

    void on_realtime_checkbox_stateChanged(int arg1);

    void on_info_button_clicked();

    void on_setting_button_clicked();

    void on_home_button_clicked();

    void on_save_button_clicked();

    void on_auto_range_checkbox_clicked(bool checked);

    void on_horizontalSlider_valueChanged(int value);

    void on_connect_button_clicked();

    void on_config_save_clicked();

    void on_config_open_clicked();

    void on_back_bt_clicked();

    void on_flash_fw_clicked();

private:
    Setting setting_ui;
    void creat_chart();
    Ui::MainWindow *ui;
    Chart *heart_beat_chart;
    Chart *bool_chart;
    Chart *pwv_chart;
    QCustomChart *evteck_chart;
    ChartView *heart_beat_chart_view;
    ChartView *bool_chart_view;
    ChartView *pwv_chart_view;
    QChartView *evteck_chart_view;
    int m_listCount;
    int m_valueMax;
    int m_valueCount;
    QString file_name;
    QFile file;
    QFile file_setting;
    QTextStream stream_in;
    QTimer *timer;
    QList<QString> data_read_file;
    QList<float> file_time;
    QList<float> data_sensor[NUM_CHANEL];
    QList<float> fdata_sensor[NUM_CHANEL];
    uint32_t num_sensor_val;
    QStringList sensor_list;
    QList<QPointF> data[9];
    QQueue<QPointF> sensor0;
    QByteArray read_buff;
    QByteArray     s1_buff;
    QList<QPointF> draw_points;
    // Line Series
    QLineSeries *heart_beat_series;
    QLineSeries *bool_series;
    QLineSeries *pwv_series;
    QLineSeries *sensor_series[4];
    bool is_setting_display;
    Save_Data *save_data;
    QTime time;
    // network connection
    QTimer *update_sensor_value;
    QTimer *gen_data;
    QTimer *fps_timer;
    QElapsedTimer m_fpsTimer;
    uint32_t fps;

};
#endif // MAINWINDOW_H
