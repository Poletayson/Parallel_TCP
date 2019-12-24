#include "mainwindow.h"

#include <QApplication>
#include <QThread>
#include <customer.h>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    //создаем объекты и перемещаем в потоки
    QThread* customerThread = new QThread();
    Customer* customer = new Customer ();
    QObject::connect(customerThread,SIGNAL(started()), customer, SLOT(run()));
    QObject::connect(customer, SIGNAL(finished()), customerThread, SLOT(terminate()));
    customer->moveToThread(customerThread);


    customerThread->start();



    MainWindow w;
    w.show();
    return a.exec();
}
