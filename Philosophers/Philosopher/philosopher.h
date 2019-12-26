#ifndef COURIER_H
#define COURIER_H

#include <QObject>
#include <QSharedMemory>
#include <QFile>
#include <QFileDevice>
#include <QTextStream>
#include <QDataStream>
#include <QThread>
#include <QTcpSocket>
#include <qcanal.h>
//#include <qcanal.h>

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
    //0 - левая, 1 - правая
    //0 - взять, 1 - отдать
    void slotSend(int forkNum, int action);    //передаем на сервер, какую вилку и что с ней делать
    void slotConnected   ();

private:
    QCanal *serverCanal;

    QTcpSocket *socket;
    QTcpSocket *socketService;
    quint16  nextBlockSize;


    int first;
    int second;
    int id;


signals:
    void finished();
};

#endif // COURIER_H
