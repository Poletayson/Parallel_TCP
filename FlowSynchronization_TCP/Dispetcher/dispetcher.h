#ifndef DISPETCHER_H
#define DISPETCHER_H

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

class Dispetcher : public QObject
{
    Q_OBJECT
public:
    explicit Dispetcher(QObject *parent = nullptr);
    void toFile (QString str);
    //void read();

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

#endif // DISPETCHER_H
