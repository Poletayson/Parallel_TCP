#ifndef STORAGE_H
#define STORAGE_H

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

private:
    QTcpSocket *socket;

    int count = 150;
    const int NEED = 10;


signals:
    void finished();
};

#endif // STORAGE_H
