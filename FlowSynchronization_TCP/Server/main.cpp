#include "mainwindow.h"

#include <QApplication>
#include <QThread>
#include <server.h>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    //создаем объекты и перемещаем в потоки


    //QThread* serverThread = new QThread();
    Server* server = new Server ();
//    QObject::connect(dispetcherThread,SIGNAL(started()), dispetcher, SLOT(run()));
//    QObject::connect(dispetcher, SIGNAL(finished()), dispetcherThread, SLOT(terminate()));
//    dispetcher->moveToThread(dispetcherThread);



//    customerThread->start();


    MainWindow w;
    w.show();
    return a.exec();
}
