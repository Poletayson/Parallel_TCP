#ifndef MASTER_H
#define MASTER_H

#include <QObject>
#include <QSharedMemory>
#include <QFile>
#include <QFileDevice>
#include <QTextStream>
#include <QDataStream>
#include <QTcpSocket>
#include <QThread>
#include <message.h>
#include <qcanal.h>

class Master : public QObject
{
    Q_OBJECT
public:
    explicit Master(QObject *parent = nullptr);
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

#endif // MASTER_H
