#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "QPixmap"
#include "QDateTimeAxis"
#include "QValueAxis"
#include "QFileDialog"
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->progressBar->setVisible(false);
    QPixmap p_icon(":/image/prefix1/icon-removebg-preview.png");
    QPixmap p_icon1(":/image/prefix1/config-removebg-preview.png");
    ui->icon->setPixmap(p_icon.scaled(ui->icon->width(),ui->icon->height(),Qt::KeepAspectRatio));
    ui->icon1->setPixmap(p_icon1.scaled(ui->icon1->width(),ui->icon1->height(),Qt::KeepAspectRatio));
    this->creat_chart();
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_BloodPressureTab_currentChanged(int index)
{
    switch (index) {
    case 0:
        ui->windown_title->setText("Idea for software: Blood pressure");
        break;
    case 1:
        ui->windown_title->setText("Idea for software: Pulse wave velocity");
        break;
    case 2:
        ui->windown_title->setText("Idea for software: Wave Form");
        break;
    default:
        break;
    }
}

void MainWindow::on_open_button_clicked()
{
    file_name = QFileDialog::getOpenFileName(this,
                                             tr("Open File"), "data_test/", tr("Files (*.txt *.csv)"));

    this->file.setFileName(file_name);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
            return;
    QTextStream stream(&this->file);
    int i = 0;
    uint64_t size = file.size();
    uint64_t is_load = 0;
    ui->progressBar->setVisible(true);
    for(uint16_t j = 0;j<8;j++){
        this->data[j].clear();
    }
    while (!stream.atEnd()){
        QString line = stream.readLine();
        is_load += line.size();
        QStringList values = line.split(QLatin1Char('\t'),Qt::SkipEmptyParts);
        for(uint16_t j = 0;j<8;j++){
            QPointF p((qreal) i, values.at(j).toDouble());
            this->data[j].push_back(p);
        }
        i++;
        double percent = (((double)is_load)/((double)size));
        ui->progressBar->setValue((uint16_t)(percent*100));
    }
    ui->progressBar->setValue(100);
    ui->progressBar->setVisible(false);
    this->file.close();
    qDebug()<<"end";
}

void MainWindow::creat_chart()
{

    this->heart_beat_chart = new Chart();
    this->bool_chart = new Chart();
    this->pwv_chart = new Chart();
    this->evteck_chart = new Chart();
    this->sensor_series[0] = new QLineSeries();
    this->evteck_chart->addSeries(this->sensor_series[0]);
    evteck_chart->setTitle("Evteck Chart");
    evteck_chart->setAnimationOptions(QChart::SeriesAnimations);
    evteck_chart->legend()->hide();
    evteck_chart->createDefaultAxes();
    this->heart_beat_chart_view = new ChartView(this->heart_beat_chart);
    this->bool_chart_view = new ChartView(this->bool_chart);
    this->pwv_chart_view = new ChartView(this->pwv_chart);
    this->evteck_chart_view = new ChartView(this->evteck_chart);
    ui->heart_beat_view->addWidget(this->heart_beat_chart_view);
    ui->PWV_view->addWidget(this->pwv_chart_view);
    ui->blood_pressure_view->addWidget(this->bool_chart_view);
    ui->wave_form_view->addWidget(this->evteck_chart_view);
}


void MainWindow::on_start_draw_chart_clicked()
{
    evteck_chart->removeAllSeries();
    if(ui->s1_raw_checkbox->isChecked()){
        this->sensor_series[0] = new QLineSeries();
        *this->sensor_series[0]<<this->data[2];
        this->evteck_chart->addSeries(this->sensor_series[0]);
    }
    if(ui->s1_filter_checkbox->isChecked()){
        this->sensor_series[1] = new QLineSeries();
        *this->sensor_series[1]<<this->data[6];
        this->evteck_chart->addSeries(this->sensor_series[1]);
    }
    if(ui->bandstop_checkbox->isChecked()){

    }
    if(ui->v_to_h_checkbox->isChecked()){

    }
    evteck_chart->setTitle("Evteck Chart");
    evteck_chart->setAnimationOptions(QChart::SeriesAnimations);
    evteck_chart->createDefaultAxes();
    this->evteck_chart->update();
}

void MainWindow::chart_update()
{

}


void MainWindow::on_stop_draw_char_clicked()
{
}

