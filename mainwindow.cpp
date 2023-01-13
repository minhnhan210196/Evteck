#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "QPixmap"
#include "QDateTimeAxis"
#include "QValueAxis"
#include "QFileDialog"
#include "QTimer"

QTimer *my_timer;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    QPixmap p_icon(":/image/prefix1/icon-removebg-preview.png");
    QPixmap p_icon1(":/image/prefix1/config-removebg-preview.png");
    ui->icon->setPixmap(p_icon.scaled(ui->icon->width(),ui->icon->height(),Qt::KeepAspectRatio));
    ui->icon1->setPixmap(p_icon1.scaled(ui->icon1->width(),ui->icon1->height(),Qt::KeepAspectRatio));
    this->creat_chart();
    my_timer = new QTimer(this);
    my_timer->setInterval(1);
    connect(my_timer,SIGNAL(timeout()),this,SLOT(chart_update()));
    my_timer->stop();

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
                                             tr("Open File"), "C:/Users/MinhNhan/Desktop/EVTeck/MONTPELIER_BiaMedical-master/HeartSignal", tr("Files (*.txt *.csv)"));

    this->file.setFileName(file_name);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
            return;
}

void MainWindow::creat_chart()
{
    this->heart_beat_chart = new Chart();
    this->bool_chart = new Chart();
    this->pwv_chart = new Chart();

    this->evteck_chart = new Evteck_Chart();
    ui->heart_beat_view->addWidget(this->heart_beat_chart->get_chart_view());
    ui->PWV_view->addWidget(this->pwv_chart->get_chart_view());
    ui->blood_pressure_view->addWidget(this->bool_chart->get_chart_view());
    ui->wave_form_view->addWidget(this->evteck_chart->get_chart_view());
}


void MainWindow::on_start_draw_chart_clicked()
{
    my_timer->start();
    qDebug()<<"timer start\n";
}

void MainWindow::chart_update()
{
    static uint32_t idex = 0;
    for(uint32_t i = 0;i<10;i++){
    if(this->file.isOpen() && !this->file.atEnd()){
        QString line = file.readLine();
        if (line.startsWith("B") || line.startsWith("\t")){

        }
        else{
            QStringList values = line.split(',', Qt::SkipEmptyParts);
            bool to_number = false;
            double num = values[4].toDouble(&to_number);
            if(to_number == false){
                goto endtimer;
            }
            this->evteck_chart->append(idex,num);
            idex++;
        }
        for(uint32_t j = 0;j<100;j++){
            if(!this->file.atEnd()){
                file.readLine();
            }
            else{
                break;
            }
        }
    }
    else{
endtimer:
        my_timer->stop();
        qDebug()<<"timer stop\n";
    }
    }
    this->evteck_chart->update();
    //qDebug()<<"timer\n";
}


void MainWindow::on_stop_draw_char_clicked()
{
    my_timer->stop();
}

