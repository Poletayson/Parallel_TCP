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

class Courier : public QObject
{
    Q_OBJECT
public:
    explicit Courier(QObject *parent = nullptr);
    void toFile (QString str);

public slots:
    void run ();

private slots:
    void slotReadyRead   ();
    void slotSend(int to, int code);    //передаем на сервер, кому и что
    void slotConnected   ();

private:
    QCanal *serverCanal;

    QTcpSocket *socket;
    QTcpSocket *socketService;
    quint16  nextBlockSize;

    int from;
    int code;


signals:
    void finished();
};

#endif // COURIER_H
