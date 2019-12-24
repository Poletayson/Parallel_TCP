#include "mainwindow.h"

#include <QApplication>
#include <QThread>
#include <courier.h>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    //создаем объекты и перемещаем в потоки

    QThread* courierThread = new QThread();
    Courier* courier = new Courier ();
    QObject::connect(courierThread,SIGNAL(started()), courier, SLOT(run()));
    QObject::connect(courier, SIGNAL(finished()), courierThread, SLOT(terminate()));
    courier->moveToThread(courierThread);

    courierThread->start();


    MainWindow w;
    w.show();
    return a.exec();
}
