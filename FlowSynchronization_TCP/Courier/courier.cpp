#include "courier.h"

Courier::Courier(QObject *parent) : QObject(parent)
{
    socket = new QTcpSocket();
    socket->connectToHost("localhost", 2140);
    connect(socket, SIGNAL(connected()), SLOT(slotConnected()));
    //connect(socket, SIGNAL(readyRead()), SLOT(slotReadyRead()));

    socketService = new QTcpSocket();
    socketService->connectToHost("localhost", 2141);

    from = 0;
    code = 0;
    nextBlockSize = 0;
    serverCanal = new QCanal ("ServerCanal"); //канал, чтобы знать сколько сокетов
}


void Courier::run()
{
    QThread::msleep(Message::DELAY);    //небольшая пауза
    while (from != Message::DISPATCHER && code != Message::READY){    //пока диспетчер не освободится
        socket->waitForReadyRead(); //ждем пока диспетчер не будет готов
        slotReadyRead();    //читаем что нам прислали
    }

    slotSend(Message::DISPATCHER, Message::MAKE_ORDER); //делаем заказ
    toFile("заказал стул");

    socket->waitForReadyRead(); //ждем когда ответят
    QThread::msleep(Message::DELAY);//небольшая пауза
    slotReadyRead();    //читаем что нам прислали

    if (code == Message::REJECTION)
        toFile("получил отказ");
    else if(code == Message::ORDER_COMPLETE){
        toFile("получил готовый заказ");

        slotSend(Message::COURIER, Message::MONEY_TRANSFER);    //передаем деньги
        toFile("передал деньги");
    }


    while (true) {
        //пока в канал мастера не поступит заказ
        socket->waitForReadyRead(); //ждем когда ответят
        QThread::msleep(Message::DELAY);//небольшая пауза
        slotReadyRead();    //читаем что нам прислали

        if (code == Message::ORDER_COMPLETE){
            toFile("получил заказ");
            slotSend(Message::CUSTOMER, Message::ORDER_COMPLETE);    //сообщает покупателю
            toFile("передал заказ");

            socket->waitForReadyRead(); //ждем когда ответят
            QThread::msleep(Message::DELAY);//небольшая пауза
            slotReadyRead();    //читаем что нам прислали
            if (code == Message::MONEY_TRANSFER){
                toFile("получил деньги");
                slotSend(Message::DISPATCHER, Message::MONEY_TRANSFER);    //передаем деньги диспетчеру
                toFile("передал деньги");
            }
            else{
                toFile("Что-то не то");
            }
        }
        else{
            toFile("Что-то не то");
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
    in.setVersion(QDataStream::Qt_5_13);
    int to;
    in >> to >> from >> code;   //кому, от кого, код
}

//передаем на сервер, кому и что
void Courier::slotSend(int to, int code)
{
    QByteArray  arrBlock;
    QDataStream out(&arrBlock, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_13);
    out << to << Message::COURIER << code;      //кому, от кого, код

    out.device()->seek(0);
    //out << quint16(arrBlock.size() - sizeof(quint16));

    socket->write(arrBlock);
}

void Courier::slotConnected()
{

}
