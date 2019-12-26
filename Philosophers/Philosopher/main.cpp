#include "mainwindow.h"

#include <QApplication>
#include <QThread>
#include <philosopher.h>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    //создаем объекты и перемещаем в потоки

    QThread* courierThread = new QThread();
    Philosopher* courier = new Philosopher ();
    QObject::connect(courierThread,SIGNAL(started()), courier, SLOT(run()));
    QObject::connect(courier, SIGNAL(finished()), courierThread, SLOT(terminate()));
    courier->moveToThread(courierThread);

    courierThread->start();


    MainWindow w;
    w.show();
    return a.exec();
}
