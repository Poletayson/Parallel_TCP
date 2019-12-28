#include "storage.h"

#include <QHostAddress>

Storage::Storage(QObject *parent) : QObject(parent)
{

}

void Storage::run()
{
    socket = new QTcpSocket();
    socket->connectToHost(QHostAddress("127.0.0.1"), 2140);
    connect(socket, SIGNAL(readyRead()), SLOT(slotReadyRead()));
}

void Storage::toFile(QString str)
{
    QFile File ("Storage.txt");
    while (!File.open(QFile::Append));
    QTextStream stream (&File);
    stream <<"Склад: " + str + "\n";
    File.close();
}

void Storage::slotReadyRead()
{
    QDataStream in(socket);
    in.setVersion(QDataStream::Qt_5_11);
    int code;
    in >> code;
    if (code == Message::COMPLETE){
        toFile("комплект изделий получен");
    }
}

