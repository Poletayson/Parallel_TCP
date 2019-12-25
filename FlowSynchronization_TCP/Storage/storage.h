#ifndef STORAGE_H
#define STORAGE_H

#include <QObject>
#include <QSharedMemory>
#include <QFile>
#include <QFileDevice>
#include <QTextStream>
#include <QDataStream>
#include <QThread>
#include <QTcpSocket>
#include <message.h>
#include <qcanal.h>

class Storage : public QObject
{
    Q_OBJECT
public:
    explicit Storage(QObject *parent = nullptr);
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

    int count = 150;
    const int NEED = 10;


signals:
    void finished();
};

#endif // STORAGE_H
