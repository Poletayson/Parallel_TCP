#include "dispetcher.h"



Dispetcher::Dispetcher(QObject *parent) : QObject(parent)
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

void Dispetcher::run(){
    while (serverCanal->get() < 3);  //ждем мастера
    while (true) {
        slotSend(Message::CUSTOMER, Message::READY);    //говорим покупателю что готовы
        while (from != Message::CUSTOMER && code != Message::MAKE_ORDER){    //пока нет заказа
            socket->waitForReadyRead(); //ждем пока диспетчер не будет готов
            slotReadyRead();    //читаем что нам прислали
        }
        QThread::msleep(Message::DELAY);


        if (code == Message::MAKE_ORDER){
            toFile("Получил заказ");
            slotSend(Message::MASTER, Message::MAKE_ORDER);    //передает заказ мастеру
//            while (masterCanal->get().getType() != Message::EMPTY);
//            QThread::msleep(Message::DELAY);
//            //dispatcherCanalOrder->unlockCanal();
//            masterCanal->put(Message::MAKE_ORDER, QVariant("Стул"));
            toFile("Передал заказ мастеру");
            socket->waitForReadyRead(); //ждем когда ответят
            slotReadyRead();    //читаем что нам прислали
            QThread::msleep(Message::DELAY);

            if (code == Message::MONEY_TRANSFER){
                toFile("Получил деньги");
                slotSend(Message::CUSTOMER, Message::READY);    //говорим покупателю что готовы
//                while (from != Message::CUSTOMER && code != Message::MAKE_ORDER){    //пока нет заказа
//                    socket->waitForReadyRead(); //ждем пока диспетчер не будет готов
//                    slotReadyRead();    //читаем что нам прислали
//                }
            }
            else{
                slotSend(Message::CUSTOMER, Message::REJECTION);    //передаем отказ
                toFile("отказ передан заказчику");

                while (from != Message::CUSTOMER && code != Message::READY){    //ждем когда заказчик будет готов
                    socket->waitForReadyRead(); //ждем пока диспетчер не будет готов
                    slotReadyRead();    //читаем что нам прислали
                }
                slotSend(Message::CUSTOMER, Message::READY);    //сами говорим заказчику что готовы
            }

        }
        else{
            toFile("Что-то не то: ");
        }
        //slotSend(Message::CUSTOMER, Message::READY);    //сообщаем о готовности

    }

}

void Dispetcher::toFile(QString str)
{
    QFile File ("Dispetcher.txt");
    while (!File.open(QFile::Append));
    QTextStream stream (&File);
    stream <<"Диспетчер: " + str + "\n";
    File.close();
}
void Dispetcher::slotReadyRead()
{
    QDataStream in(socket);
    in.setVersion(QDataStream::Qt_5_13);
    int to;
    in >> to >> from >> code;   //кому, от кого, код
}

//передаем на сервер, кому и что
void Dispetcher::slotSend(int to, int code)
{
    QByteArray  arrBlock;
    QDataStream out(&arrBlock, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_13);
    out << to << Message::DISPATCHER << code;      //кому, от кого, код

    out.device()->seek(0);
    //out << quint16(arrBlock.size() - sizeof(quint16));

    socket->write(arrBlock);
}

void Dispetcher::slotConnected()
{

}