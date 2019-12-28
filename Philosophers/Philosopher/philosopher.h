#ifndef COURIER_H
#define COURIER_H

#include <QObject>
#include <QSharedMemory>
#include <QSystemSemaphore>
#include <QFile>
#include <QFileDevice>
#include <QTextStream>
#include <QDataStream>
#include <QThread>
#include <QTcpSocket>
#include <../Server/message.h>

class Philosopher : public QObject
{
    Q_OBJECT
public:
    explicit Philosopher(QObject *parent = nullptr);
    void toFile (QString str);

public slots:
    void run ();

private slots:
    void slotReadyRead   ();
    void slotSend(int what, int code);    //передаем на сервер, какую вилку и что с ней делать
    void slotConnected   ();

private:
    //QCanal *serverCanal;

    QTcpSocket *socket;
    QSystemSemaphore *semaphoreServer;
    QSystemSemaphore *semaphorePhilosopher;
    quint16  nextBlockSize;


    int first;
    int second;
    int id;


signals:
    void finished();
};

#endif // COURIER_H
