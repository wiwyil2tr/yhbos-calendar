#include <clocale>
#include <QApplication>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    setlocale(LC_ALL, "zh_CN.UTF-8");

    QApplication app(argc, argv);
    app.setApplicationName("yhbos-calendar");

    MainWindow w;
    w.show();

    return app.exec();
}