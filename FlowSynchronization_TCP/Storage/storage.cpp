#include "storage.h"

#include <QHostAddress>

Storage::Storage(QObject *parent) : QObject(parent)
{

    from = 0;
    code = 0;
    nextBlockSize = 0;

}


void Storage::run()
{
    socket = new QTcpSocket();
    socket->connectToHost(QHostAddress("127.0.0.1"), 2140);
    connect(socket, SIGNAL(connected()), SLOT(slotConnected()));
    //connect(socket, SIGNAL(readyRead()), SLOT(slotReadyRead()));

    socketService = new QTcpSocket();
    socketService->connectToHost(QHostAddress("127.0.0.1"), 2141);
    serverCanal = new QCanal ("ServerCanal"); //канал, чтобы знать сколько сокетов

    while (true) {
        socket->waitForReadyRead(); //ждем пока не закажут материалы
        slotReadyRead();    //читаем что нам прислали
        qDebug() << "Запрос мастера: " << from <<  " код: " << code;
        QThread::msleep(Message::DELAY);
        //заказали материалы
        if (code == Message::MATERIALS_REQUEST){
            toFile("поступил заказ на материалы");
            QThread::msleep(Message::DELAY);
            if (count >= NEED){
                count -= NEED;
                slotSend(Message::MASTER, Message::MATERIALS_ARE);    //передаем материалы мастеру
                toFile("передал материалы. Осталось: " + QString::number(count));
            }
            else{
                slotSend(Message::MASTER, Message::MATERIALS_ARE_NOT);  //передаем отказ
                toFile("материалов недостаточно. Осталось: " + QString::number(count));
            }
        }
        else{
            qDebug() << "Что-то не то";
            toFile("Что-то не то");
        }
    }
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
    int to;
    in >> to >> from >> code;   //кому, от кого, код
}

//передаем на сервер, кому и что
void Storage::slotSend(int to, int code)
{
    QByteArray  arrBlock;
    QDataStream out(&arrBlock, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_11);
    out << to << Message::STORAGE << code;      //кому, от кого, код

    out.device()->seek(0);
    //out << quint16(arrBlock.size() - sizeof(quint16));

    socket->write(arrBlock);
}

void Storage::slotConnected()
{

}

