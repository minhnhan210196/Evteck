#ifndef SETTING_H
#define SETTING_H

#include <QWidget>
#include <QTcpSocket>
#include <QFile>
#include <QTextStream>

namespace Ui {
class Setting;
}

class Setting : public QWidget
{
    Q_OBJECT

public:
    explicit Setting(QWidget *parent = nullptr);
    ~Setting();
public slots:
    void connected();
    void disconnected();
    void bytesWritten(qint64 bytes);
    void readyRead();
private slots:
    void on_connect_bt_clicked();

    void on_file_bt_clicked();

    void on_flash_bt_clicked();

private:
    Ui::Setting *ui;
    QTcpSocket *p_network = NULL;
    QFile file;
};

#endif // SETTING_H
