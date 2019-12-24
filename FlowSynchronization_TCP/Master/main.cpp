#include "mainwindow.h"

#include <QApplication>
#include <QThread>
#include <master.h>


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    //создаем объекты и перемещаем в потоки

    QThread* masterThread = new QThread();
    Master* master = new Master ();
    QObject::connect(masterThread,SIGNAL(started()), master, SLOT(run()));
    QObject::connect(master, SIGNAL(finished()), masterThread, SLOT(terminate()));
    master->moveToThread(masterThread);

    masterThread->start();

    MainWindow w;
    w.show();
    return a.exec();
}
