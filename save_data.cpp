#include "save_data.h"

Save_Data::Save_Data(const char *file)
{
    this->file_name = file;
    this->file.setFileName(file);
    this->file.open(QIODevice::ReadWrite | QIODevice::Text);
}

Save_Data::~Save_Data()
{

}

void Save_Data::write(const QByteArray &data)
{
    if(this->file.isOpen()){
        this->file.write(data);
    }
    else{
        this->file.setFileName(file_name);
        this->file.open(QIODevice::Append | QIODevice::Text);
        this->file.write(data);
    }
}

void Save_Data::save()
{
    file.close();
}
