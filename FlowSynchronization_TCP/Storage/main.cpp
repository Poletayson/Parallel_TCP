#include "mainwindow.h"

#include <QApplication>
#include <QThread>
#include <storage.h>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    //создаем объекты и перемещаем в потоки

    QThread* storageThread = new QThread();
    Storage* storage = new Storage ();
    QObject::connect(storageThread,SIGNAL(started()), storage, SLOT(run()));
    QObject::connect(storage, SIGNAL(finished()), storageThread, SLOT(terminate()));
    storage->moveToThread(storageThread);

    storageThread->start();



    MainWindow w;
    w.show();
    return a.exec();
}
