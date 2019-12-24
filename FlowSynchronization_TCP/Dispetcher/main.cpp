#include "mainwindow.h"

#include <QApplication>
#include <QThread>
#include <dispetcher.h>


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    //создаем объекты и перемещаем в потоки

    QThread* dispetcherThread = new QThread();
    Dispetcher* dispetcher = new Dispetcher ();
    QObject::connect(dispetcherThread,SIGNAL(started()), dispetcher, SLOT(run()));
    QObject::connect(dispetcher, SIGNAL(finished()), dispetcherThread, SLOT(terminate()));
    dispetcher->moveToThread(dispetcherThread);

    dispetcherThread->start();

    MainWindow w;
    w.show();
    return a.exec();
}
