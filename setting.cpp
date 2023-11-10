#include "setting.h"
#include "ui_setting.h"
#include <QFileDialog>
#include <QString>
#include <QDebug>
#include <dialog.h>

Setting::Setting(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Setting)
{
    ui->setupUi(this);
    ui->progressBar->setVisible(false);
    ui->flash_bt->setEnabled(false);
    p_network = new QTcpSocket();
    connect(p_network, SIGNAL(connected()),this, SLOT(connected()));
    connect(p_network, SIGNAL(disconnected()),this, SLOT(disconnected()));
    connect(p_network, SIGNAL(bytesWritten(qint64)),this, SLOT(bytesWritten(qint64)));
    connect(p_network, SIGNAL(readyRead()),this, SLOT(readyRead()));
}

Setting::~Setting()
{
    delete ui;
}

void Setting::connected()
{
    ui->connect_bt->setText("Connected");
    ui->flash_bt->setEnabled(true);
}

void Setting::disconnected()
{
    ui->connect_bt->setText("Connect");
    ui->flash_bt->setEnabled(false);
}

void Setting::bytesWritten(qint64 bytes)
{
    (void) bytes;
}

void Setting::readyRead()
{

}

void Setting::run()
{

}

void Setting::on_connect_bt_clicked()
{
    if(!p_network->isOpen()){
        p_network->connectToHost(ui->ip_addr->text(),4000);
        if(!p_network->waitForConnected(5000))
        {
            qDebug() << "Error: " << p_network->errorString();
            return;
        }
    }
    else{
        p_network->close();
    }
}


void Setting::on_file_bt_clicked()
{
    QString file_name = QFileDialog::getOpenFileName(this,
                                             tr("Open File"), "data_test/", tr("Files (*.bin)"));
    this->ui->file_addr->setText(file_name);

    this->file.setFileName(file_name);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
            return;
    QTextStream stream(&this->file);

}


void Setting::on_flash_bt_clicked()
{
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)){
            Dialog *dialog = new Dialog();
            dialog->set_text("Error");
            return;
    }
    QTextStream stream(&this->file);
    connect(this->timer,SIGNAL(timeout),this,SLOT(run));
    this->timer.start(1);
}

