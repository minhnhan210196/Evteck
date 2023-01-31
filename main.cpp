#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
// add ui
    QApplication a(argc, argv);
    MainWindow w;
    w.setWindowTitle("eV-Technologies");
    w.show();
    return a.exec();
}
