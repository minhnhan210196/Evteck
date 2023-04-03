#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "QPixmap"
#include "QDateTimeAxis"
#include "QValueAxis"
#include "QFileDialog"
#include "QPropertyAnimation"
#include "QAbstractAxis"
#include "stdint.h"
#include "math.h"
#include "stdlib.h"
#include "QJsonDocument"
#include "QJsonObject"
#include "QJsonArray"
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
    this->file_setting.setFileName("config.json");
    this->file_setting.open(QIODevice::ReadWrite | QIODevice::Text);

    QByteArray json_raw = file_setting.readAll();

    this->file_setting.close();

    QJsonDocument jsonResponse = QJsonDocument::fromJson(json_raw);

    if(jsonResponse.isNull()){
        qDebug()<<"File config empty";
        QJsonObject json_obj;
        json_obj["ip"] = "192.168.1.10";
        json_obj["port"] = 1234;
        json_obj["bot freq"] = 48;
        json_obj["top freq"] = 52;
        json_obj["order"] = 30;
        json_obj["band width"] = 4;
        QJsonArray freq_array = {50,100,150,200};
        json_obj["freq"] = freq_array ;
        json_obj["r"] = 1E-05;
        json_obj["q"] = 0.7;
        json_obj["gain"] = 2;
        jsonResponse.setObject(json_obj);
        this->file_setting.open(QIODevice::ReadWrite | QIODevice::Text);
        this->file_setting.write(jsonResponse.toJson(QJsonDocument::Indented));
        this->file_setting.close();
    }
    else{
        QJsonObject json_obj = jsonResponse.object();
        ui->ip_host->setText(json_obj["ip"].toString());
        ui->port_host->setText(QString::number(json_obj["port"].toInt()));
        ui->bandpass_freq_bottom->setText(QString::number(json_obj["bot freq"].toInt()));
        ui->bandpass_freq_top->setText(QString::number(json_obj["top freq"].toInt()));
        ui->bandpass_oder->setText(QString::number(json_obj["order"].toInt()));
        ui->bandstop_width->setText(QString::number(json_obj["band width"].toInt()));
        QJsonArray freq_array = json_obj["freq"].toArray();
//        for(uint16_t i = 0;freq_array.size();i++){
//            if(freq_array[i].toInt() == 50)
//                ui->bandstop_checkbox_1->setCheckable(true);
//            if(freq_array[i].toInt() == 100)
//                ui->bandstop_checkbox_2->setCheckable(true);
//            if(freq_array[i].toInt() == 150)
//                ui->bandstop_checkbox_3->setCheckable(true);
//            if(freq_array[i].toInt() == 200)
//                ui->bandstop_checkbox_4->setCheckable(true);
//        }
        ui->kalman_q->setText(QString::number(json_obj["q"].toDouble()));
        ui->kalman_r->setText(QString::number(json_obj["r"].toDouble()));
        ui->gain->setCurrentText(QString::number(json_obj["gain"].toInt()));
    }

    this->creat_chart();
    this->ui->setting_display->setVisible(false);
    this->is_setting_display = false;
    this->p_network = new QTcpSocket(this);

    connect(this->p_network, SIGNAL(connected()),this, SLOT(connected()));
    connect(this->p_network, SIGNAL(disconnected()),this, SLOT(disconnected()));
    connect(this->p_network, SIGNAL(bytesWritten(qint64)),this, SLOT(bytesWritten(qint64)));
    connect(this->p_network, SIGNAL(readyRead()),this, SLOT(readyRead()));

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
    this->heart_beat_series = new QLineSeries();
    this->heart_beat_chart->addSeries(this->heart_beat_series);
    heart_beat_chart->setTitle("Heart beat Chart");
    heart_beat_chart->setAnimationOptions(QChart::SeriesAnimations);
    heart_beat_chart->legend()->hide();
    heart_beat_chart->createDefaultAxes();
    /*Creat boold chart*/
    this->bool_chart = new Chart();
    this->bool_series = new QLineSeries();
    this->bool_chart->addSeries(this->bool_series);
    bool_chart->setTitle("Boold Chart");
    bool_chart->setAnimationOptions(QChart::SeriesAnimations);
    bool_chart->legend()->hide();
    bool_chart->createDefaultAxes();
    /*End Creat boold chart*/
    /*Creat pwv chart*/
    this->pwv_chart = new Chart();
    this->pwv_series = new QLineSeries();
    this->pwv_chart->addSeries(this->pwv_series);
    pwv_chart->setTitle("PWV Chart");
    pwv_chart->setAnimationOptions(QChart::SeriesAnimations);
    pwv_chart->legend()->hide();
    pwv_chart->createDefaultAxes();
    /*End creat pwv chart*/
    /*Creat evteck chart*/
    this->evteck_chart = new Chart();
    this->sensor_series[0] = new QLineSeries();
    this->evteck_chart->addSeries(this->sensor_series[0]);
    evteck_chart->setTitle("Evteck Chart");
    evteck_chart->setAnimationOptions(QChart::SeriesAnimations);
    evteck_chart->legend()->hide();
    evteck_chart->createDefaultAxes();
    if(ui->v_to_h_checkbox->isChecked()){
        this->evteck_chart->axisY()->setTitleText("uT");
    }
    else{
        this->evteck_chart->axisY()->setTitleText("mV");
    }

    /*End creat evteck chart*/
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
    if(ui->v_to_h_checkbox->isChecked()){
        this->evteck_chart->axisY()->setTitleText("uT");
    }
    else{
        this->evteck_chart->axisY()->setTitleText("mV");
    }
    this->evteck_chart->update();
}

void MainWindow::chart_update()
{

}

void MainWindow::connected()
{
    qDebug() << "Connected";
    ui->connect_button->setIcon(QIcon(":/image/image/connect.png"));
    ui->connect_button->setIconSize(QSize(16,16));
}

void MainWindow::disconnected()
{
    qDebug() << "Disconnected";
    ui->connect_button->setIcon(QIcon(":/image/image/disconnect.png"));
    ui->connect_button->setIconSize(QSize(20,20));
}

void MainWindow::bytesWritten(qint64 bytes)
{
    qDebug() << bytes << " bytes written...";
}

void MainWindow::readyRead()
{
    qDebug() << "reading...";

    // read the data from the socket
    qDebug() << this->p_network->readAll();
}


void MainWindow::on_stop_draw_char_clicked()
{
}


void MainWindow::on_clear_button_clicked()
{
    evteck_chart->removeAllSeries();
    this->sensor_series[0] = new QLineSeries();
    evteck_chart->setTitle("Evteck Chart");
    evteck_chart->setAnimationOptions(QChart::SeriesAnimations);
    evteck_chart->createDefaultAxes();
    this->evteck_chart->update();
}


void MainWindow::on_s1_raw_checkbox_stateChanged(int arg1)
{
    if(arg1){

    }
    else{

    }
}


void MainWindow::on_s1_filter_checkbox_stateChanged(int arg1)
{
    if(arg1){

    }
    else{

    }
}


void MainWindow::on_bandpass_checkbox_stateChanged(int arg1)
{

}


void MainWindow::on_bandstop_checkbox_stateChanged(int arg1)
{
    if(arg1){

    }
    else{

    }
}


void MainWindow::on_kalman_checkbox_stateChanged(int arg1)
{
    if(arg1){

    }
    else{

    }
}


void MainWindow::on_v_to_h_checkbox_stateChanged(int arg1)
{
    if(arg1){
        this->evteck_chart->axisY()->setTitleText("uT");
        this->evteck_chart->update();
    }
    else{
        this->evteck_chart->axisY()->setTitleText("mV");
        this->evteck_chart->update();
    }
}


void MainWindow::on_realtime_checkbox_stateChanged(int arg1)
{

}


void MainWindow::on_info_button_clicked()
{

}


void MainWindow::on_setting_button_clicked()
{
    if(this->is_setting_display == false){
        this->evteck_chart_view->setVisible(false);
        this->ui->tool_view->setVisible(false);
        this->ui->setting_display->setVisible(true);
        this->is_setting_display = true;
    }

}


void MainWindow::on_home_button_clicked()
{
    if(this->is_setting_display == true){
        this->evteck_chart_view->setVisible(true);
        this->ui->tool_view->setVisible(true);
        this->ui->setting_display->setVisible(false);
        this->is_setting_display = false;
    }
}


void MainWindow::on_save_button_clicked()
{

}


void MainWindow::on_auto_range_checkbox_clicked(bool checked)
{
    if(checked){
        ui->horizontalSlider->setEnabled(false);
    }
    else{
        ui->horizontalSlider->setEnabled(true);
    }
}




void MainWindow::on_horizontalSlider_valueChanged(int value)
{
    this->ui->scroll_value->setText(QString::number(value));
}


void MainWindow::on_connect_button_clicked()
{
    if(!this->p_network->isOpen()){
        this->p_network->connectToHost(ui->ip_host->text(),ui->port_host->text().toInt());
        if(!this->p_network->waitForConnected(5000))
        {
            qDebug() << "Error: " << this->p_network->errorString();
        }
    }
    else{
        this->p_network->close();
    }
}


void MainWindow::on_config_save_clicked()
{

}


void MainWindow::on_config_open_clicked()
{
}

