#ifndef SAVE_DATA_H
#define SAVE_DATA_H

#include "QFile"
#include "QByteArray"

class Save_Data
{
public:
    Save_Data(const char *file);
    ~Save_Data();

    void write(const QByteArray &data);
    void save();
private:
    QFile file;
    QString file_name;
};

#endif // SAVE_DATA_H
