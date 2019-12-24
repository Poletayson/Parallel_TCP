#ifndef CUSTOMER_H
#define CUSTOMER_H

#include <QObject>
#include <QSharedMemory>
#include <QFile>
#include <QFileDevice>
#include <QTextStream>
#include <QDataStream>
#include <QThread>
#include <QTcpSocket>
#include <qcanal.h>

class Customer : public QObject
{
    Q_OBJECT
public:
    explicit Customer(QObject *parent = nullptr);   
    void toFile (QString str);


public slots:
    void run ();

private slots:
    void slotReadyRead   ();
    void slotSend(int to, int code);    //передаем на сервер, кому и что
    void slotConnected   ();

private:
    QCanal *serverCanal;

    //Server *server;
    QTcpSocket *socket;
    QTcpSocket *socketService;
    quint16  nextBlockSize;

    int ORDER_COUNT = 8000;

    int from;
    int code;

    int fromService;
    int codeService;


    void readService();

signals:
    void finished();


};

#endif // CUSTOMER_H
