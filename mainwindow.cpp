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

#define MAX_DISPLAY_CHART_POINTS        1000
#define NUM_DIV_POINTS                  1
#define SET_DISPLAY_POINTS              MAX_DISPLAY_CHART_POINTS/NUM_DIV_POINTS
#define TIME_UPDATE_CHART_mS            1
#define SAMPLE_TIME_mS                  0.05
QTimer *timer0;
QTcpSocket *p_network = NULL;
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
    this->fps = 0;
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
    p_network = new QTcpSocket();

    timer0 = new QTimer(this);

    connect(timer0,SIGNAL(timeout()),this,SLOT(update_senor_slot()));
    timer0->start(TIME_UPDATE_CHART_mS);

    connect(p_network, SIGNAL(connected()),this, SLOT(connected()));
    connect(p_network, SIGNAL(disconnected()),this, SLOT(disconnected()));
    connect(p_network, SIGNAL(bytesWritten(qint64)),this, SLOT(bytesWritten(qint64)));
    connect(p_network, SIGNAL(readyRead()),this, SLOT(readyRead()));

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
            //this->evteck_chart->add_PointY(values.at(j).toFloat());
//            QPointF p((qreal) i, values.at(j).toDouble());
//            this->data[j].push_back(p);
            this->data[0].append(QPointF(i++,values.at(j).toFloat()));
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
    this->evteck_chart->set_max_point(SET_DISPLAY_POINTS);
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
    this->gen_data  = new QTimer();
    connect(this->gen_data,SIGNAL(timeout()),this,SLOT(gen_data_sensor()));
    this->gen_data->start(1);
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

#define MAX_NUM_SENSOR_VAL 5000

typedef union{
    uint8_t u8_t[4];
    float fl;
}Float_Convert_t;

typedef union{
    uint8_t u8_t[2];
    uint16_t u16_t;
}Uint16_Convert_t;

void MainWindow::readyRead()
{

    //qDebug() << "reading...";
    //QByteArray data_ready = this->p_network->readAll();
//    this->read_buff.append(this->p_network->readAll());
    QByteArray data_ready = p_network->readLine();
    if(data_ready.back() == '\n' && data_ready.front() == ':'){
        qDebug() << "data ready";
        static float x = 0;
        QByteArrayList values = data_ready.split(',');
        values.pop_front();
        values.pop_back();
        for(uint32_t i = 0;i<values.count();i++){
            float y = values.at(i).toFloat();
            x++;
            if(this->draw_points.count() > this->evteck_chart->get_max_points()){
                this->draw_points.pop_front();
                this->draw_points.append(QPointF(x,y));
            }else{
                this->draw_points.append(QPointF(x,y));
            }
        }
    }
//    this->read_buff.enqueue(data_ready);

    // read the data from the socket


}

void MainWindow::update_senor_slot()
{
//    static float x = 0;
//    this->fps+= TIME_UPDATE_CHART_mS;
//    if(this->read_buff.count()>0){
//        this->s1_buff.clear();
//        this->s1_buff = this->read_buff.dequeue();
//        uint8_t header = this->s1_buff.at(0);
//        if(header == ':'){
//            s1_buff.remove(0,1);
//            QByteArrayList values = this->s1_buff.split(',');
//            this->draw_points.clear();
//            for(uint16_t i = 1;i<values.count();i+=NUM_DIV_POINTS){
//                float y = values.at(i).toFloat();
//                this->draw_points.append(QPointF(x,y));
//                if(this->draw_points.count() > this->evteck_chart->get_max_points()){
//                    this->draw_points.pop_back();
//                    this->evteck_chart->replace_series(this->draw_points);
//                    if(this->fps > 0 ){
//                        double sample_per_sec = ((double)1000/SAMPLE_TIME_mS);

//                        this->ui->sample_value->setText(QString::number(sample_per_sec));

//                        this->fps = 0;
//                    }
//                    this->draw_points.clear();
//                }
//                x+= (NUM_DIV_POINTS*SAMPLE_TIME_mS);
//            }
//        }
//    }
//    if(this->data[0].count() > this->evteck_chart->get_max_points()){
//        this->evteck_chart->replace_series(this->data[0]);
//        this->data[0].clear();
//    }


//    this->ui->max_value->setText(QString::number(this->evteck_chart->get_max_y()));
//    this->ui->min_value->setText(QString::number(this->evteck_chart->get_min_y()));
    if(this->draw_points.count() > this->evteck_chart->get_max_points() / 4){
        static QString labelText = QStringLiteral("FPS: %1");
        static int frameCount = 0;
        frameCount++;
        int elapsed = m_fpsTimer.elapsed();
        if (elapsed >= 1000) {
            elapsed = m_fpsTimer.restart();
            qreal fps = qreal(0.1 * int(10000.0 * (qreal(frameCount) / qreal(elapsed))));
            ui->fsp_label->setText(labelText.arg(QString::number(fps, 'f', 1)));
            ui->fsp_label->adjustSize();
            frameCount = 0;
        }

        this->evteck_chart->replace_series(this->draw_points);
        this->ui->max_value->setText(QString::number(this->evteck_chart->get_max_y()));
        this->ui->min_value->setText(QString::number(this->evteck_chart->get_min_y()));
    }
}

void MainWindow::gen_data_sensor()
{
    static float x = 0;
    for(uint16_t i = 0;i<this->evteck_chart->get_max_points();i++){
        x++;
        float y = 10*sin(0.2*x*3.14 + 10) + QRandomGenerator::global()->bounded(-5,5);
        this->data[0].append(QPointF(x,y));
    }
}

void MainWindow::fps_run_time()
{
   // this->fps++;
}

void MainWindow::on_stop_draw_char_clicked()
{
}


void MainWindow::on_clear_button_clicked()
{
    this->draw_points.clear();
    for(uint32_t i = 0;i<this->evteck_chart->get_max_points();i++){
        this->draw_points.push_back(QPointF(i,0));
    }
    this->evteck_chart->replace_series(this->draw_points);
}


void MainWindow::on_s1_raw_checkbox_stateChanged(int arg1)
{
    if(arg1){
        this->evteck_chart->set_data_raw_en(true);
    }
    else{
        this->evteck_chart->set_data_raw_en(false);
    }
}


void MainWindow::on_s1_filter_checkbox_stateChanged(int arg1)
{
    if(arg1){
        this->evteck_chart->set_filter_en(true);
    }
    else{
        this->evteck_chart->set_filter_en(false);
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
        this->evteck_chart->set_max_point(SET_DISPLAY_POINTS);
    }
    else{
        ui->horizontalSlider->setEnabled(true);
        this->evteck_chart->set_max_point(this->ui->horizontalSlider->value()/NUM_DIV_POINTS);
    }
}




void MainWindow::on_horizontalSlider_valueChanged(int value)
{
    this->ui->scroll_value->setText(QString::number(value));
    this->evteck_chart->set_max_point(value/NUM_DIV_POINTS);
    if(this->draw_points.count() > this->evteck_chart->get_max_points()){
        uint64_t num = this->draw_points.count() - this->evteck_chart->get_max_points();
        for(uint64_t i = 0;i<num;i++){
            this->draw_points.pop_front();
        }
    }
}


void MainWindow::on_connect_button_clicked()
{
    if(!p_network->isOpen()){
        p_network->connectToHost(ui->ip_host->text(),ui->port_host->text().toInt());
        if(!p_network->waitForConnected(5000))
        {
            qDebug() << "Error: " << p_network->errorString();
        }
    }
    else{
        p_network->close();
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

