#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "QPixmap"
#include "QDateTimeAxis"
#include "QValueAxis"
#include "QFileDialog"
#include "QPropertyAnimation"
#include "QAbstractAxis"

#include "math.h"
#include "stdlib.h"
#include "QJsonDocument"
#include "QJsonObject"
#include "QJsonArray"




MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
    ui(new Ui::MainWindow)
{


    this->save_data = new Save_Data("data.txt");

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

    update_sensor_value = new QTimer(this);

    connect(this->update_sensor_value,SIGNAL(timeout()),this,SLOT(update_senor_slot()));

    this->update_sensor_value->setInterval(1);

    this->update_sensor_value->start(1);

    this->gen_data = new QTimer(this);

    connect(this->gen_data,SIGNAL(timeout()),this,SLOT(gen_data_sensor()));
    this->gen_data->start(10);


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
            this->evteck_chart->add_PointY(values.at(j).toFloat());
//            QPointF p((qreal) i, values.at(j).toDouble());
//            this->data[j].push_back(p);
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
    this->evteck_chart = new QCustomChart();
    evteck_chart->setTitle("Evteck Chart");

    if(ui->v_to_h_checkbox->isChecked()){
        this->evteck_chart->set_axisY_title("uT");
    }
    else{
        this->evteck_chart->set_axisY_title("mV");
    }
    this->evteck_chart->set_max_point(10000);
    /*End creat evteck chart*/
    this->heart_beat_chart_view = new ChartView(this->heart_beat_chart);
    this->bool_chart_view = new ChartView(this->bool_chart);
    this->pwv_chart_view = new ChartView(this->pwv_chart);
    this->evteck_chart_view = new QChartView(this->evteck_chart);
    ui->heart_beat_view->addWidget(this->heart_beat_chart_view);
    ui->PWV_view->addWidget(this->pwv_chart_view);
    ui->blood_pressure_view->addWidget(this->bool_chart_view);
    ui->wave_form_view->addWidget(this->evteck_chart_view);
    this->num_sensor_val = 0;
}


void MainWindow::on_start_draw_chart_clicked()
{
    if(ui->bandstop_checkbox->isChecked()){

    }
    if(ui->v_to_h_checkbox->isChecked()){

    }
    if(ui->v_to_h_checkbox->isChecked()){
        this->evteck_chart->set_axisY_title("uT");
    }
    else{
        this->evteck_chart->set_axisY_title("mV");
    }
    //this->evteck_chart->update();
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

typedef union ev_float_t ev_float;

union ev_float_t{
    uint8_t data[4];
    float f;
};

#define MAX_NUM_SENSOR_VAL 10000


void MainWindow::readyRead()
{
    qDebug() << "reading...";
    QByteArray data_ready = this->p_network->readAll();
    // read the data from the socket
    //qDebug() << data_ready;
    uint8_t header = data_ready.at(0);
    uint16_t len1 = data_ready.at(1);
    uint16_t len2 = (uint8_t)data_ready.at(2);
    if(header == ':'){
        uint16_t length =  len1*256 + len2;
        qDebug()<<"header - "<<header;
        qDebug()<<"length - "<<length;
        ev_float ev_f;
        for(uint16_t i = 3;i<length ;i+=4){
            ev_f.data[0] = data_ready.at(i+0);
            ev_f.data[1] = data_ready.at(i+1);
            ev_f.data[2] = data_ready.at(i+2);
            ev_f.data[3] = data_ready.at(i+3);
            this->evteck_chart->add_PointY(ev_f.f);
        }
    }
}

void MainWindow::update_senor_slot()
{
    if(this->data[0].count() > this->evteck_chart->get_max_points()){
        this->evteck_chart->replace_series(this->data[0]);
        this->data[0].clear();
    }

}

void MainWindow::gen_data_sensor()
{
    static float x = 0;
    for(uint16_t i = 0;i<this->evteck_chart->get_max_points();i++){
        x++;
        float y = sin(0.2*x*3.14 + 10) + 2*cos(0.3*x*3.14 +2) + 3*sin(10*x*3.14);
        this->data[0].append(QPointF(x,y));
    }
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
    if(arg1){

    }
    else{

    }
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
        this->evteck_chart->set_axisY_title("uT");
        this->evteck_chart->update();
    }
    else{
        this->evteck_chart->set_axisY_title("mV");
        this->evteck_chart->update();
    }
}


void MainWindow::on_realtime_checkbox_stateChanged(int arg1)
{
    if(arg1){

    }
    else{

    }
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
        //this->evteck_chart->axisX()->setRange(0,this->sensor_series[0]->count());
        this->evteck_chart->set_max_point(10000);
    }
    else{
        ui->horizontalSlider->setEnabled(true);
        this->evteck_chart->set_max_point(this->ui->horizontalSlider->value()*1000);
    }
}




void MainWindow::on_horizontalSlider_valueChanged(int value)
{
    this->ui->scroll_value->setText(QString::number(value));
    this->evteck_chart->set_max_point(value*1000);
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
    QJsonObject json_obj;
    json_obj["ip"] = ui->ip_host->text();
    json_obj["port"] = ui->port_host->text().toInt();
    json_obj["bot freq"] = ui->bandpass_freq_bottom->text().toInt();
    json_obj["top freq"] = ui->bandpass_freq_top->text().toInt();
    json_obj["order"] = ui->bandpass_oder->text().toInt();
    json_obj["band width"] = ui->bandstop_width->text().toInt();
    QJsonArray freq_array = {50,100,150,200};
    json_obj["freq"] = freq_array ;
    json_obj["r"] = ui->kalman_r->text().toDouble();
    json_obj["q"] = ui->kalman_q->text().toDouble();
    json_obj["gain"] = ui->gain->currentText().toInt();
    QJsonDocument jsonResponse;
    jsonResponse.setObject(json_obj);
    this->file_setting.open(QIODevice::ReadWrite | QIODevice::Text);
    this->file_setting.write(jsonResponse.toJson(QJsonDocument::Indented));
    this->file_setting.close();
    qDebug()<<"Save config file";
    if(this->is_setting_display == true){
        this->evteck_chart_view->setVisible(true);
        this->ui->tool_view->setVisible(true);
        this->ui->setting_display->setVisible(false);
        this->is_setting_display = false;
    }
}


void MainWindow::on_config_open_clicked()
{
}


void MainWindow::on_back_bt_clicked()
{
    if(this->is_setting_display == true){
        this->evteck_chart_view->setVisible(true);
        this->ui->tool_view->setVisible(true);
        this->ui->setting_display->setVisible(false);
        this->is_setting_display = false;
    }
}

