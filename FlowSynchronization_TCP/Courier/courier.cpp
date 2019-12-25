#include "courier.h"

#include <QHostAddress>

Courier::Courier(QObject *parent) : QObject(parent)
{


    from = 0;
    code = 0;
    nextBlockSize = 0;

}


void Courier::run()
{
    socket = new QTcpSocket();
    socket->connectToHost(QHostAddress("127.0.0.1"), 2140);
    connect(socket, SIGNAL(connected()), SLOT(slotConnected()));
    //connect(socket, SIGNAL(readyRead()), SLOT(slotReadyRead()));

//    socketService = new QTcpSocket();
//    socketService->connectToHost("localhost", 2141);
    serverCanal = new QCanal ("ServerCanal"); //канал, чтобы знать сколько сокетов

    while (true) {
        socket->waitForReadyRead(); //ждем когда ответят
        QThread::msleep(Message::DELAY);//небольшая пауза
        slotReadyRead();    //читаем что нам прислали
        qDebug() << "Запрос мастера: " << from <<  " код: " << code;
        if (code == Message::ORDER_COMPLETE){
            toFile("получил заказ");
            slotSend(Message::CUSTOMER, Message::ORDER_COMPLETE);    //сообщает покупателю
            toFile("передал заказ");
            qDebug() << "Передал заказ, ждем деньги";

            socket->waitForReadyRead(); //ждем когда ответят
            QThread::msleep(Message::DELAY);//небольшая пауза
            slotReadyRead();    //читаем что нам прислали
            qDebug() << "Деньги от заказчика: " << from <<  " код: " << code;
            if (code == Message::MONEY_TRANSFER){
                toFile("получил деньги");
                slotSend(Message::DISPATCHER, Message::MONEY_TRANSFER);    //передаем деньги диспетчеру
                toFile("передал деньги");
            }
            else{
                qDebug() << "Что-то не то, ждалось " << Message::MONEY_TRANSFER;
                toFile("Что-то не то");
            }
        }
        else{
            qDebug() << "Что-то не то, ждалось " << Message::ORDER_COMPLETE;
            toFile("заказа нет");
        }

    }

}

void Courier::toFile(QString str)
{
    QFile File ("Courier.txt");
    while (!File.open(QFile::Append));
    QTextStream stream (&File);
    stream <<"Курьер: " + str + "\n";
    File.close();
}

void Courier::slotReadyRead()
{
    QDataStream in(socket);
    in.setVersion(QDataStream::Qt_5_11);
    int to;
    in >> to >> from >> code;   //кому, от кого, код
}

//передаем на сервер, кому и что
void Courier::slotSend(int to, int code)
{
    QByteArray  arrBlock;
    QDataStream out(&arrBlock, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_11);
    out << to << Message::COURIER << code;      //кому, от кого, код

    out.device()->seek(0);
    //out << quint16(arrBlock.size() - sizeof(quint16));

    socket->write(arrBlock);
}

void Courier::slotConnected()
{

}
