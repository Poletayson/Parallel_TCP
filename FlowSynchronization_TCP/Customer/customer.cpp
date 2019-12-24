#include "customer.h"



Customer::Customer(QObject *parent) : QObject(parent)
{
    socket = new QTcpSocket();
    socket->connectToHost("localhost", 2140);
    connect(socket, SIGNAL(connected()), SLOT(slotConnected()));
    //connect(socket, SIGNAL(readyRead()), SLOT(slotReadyRead()));

    socketService = new QTcpSocket();
    socketService->connectToHost("localhost", 2141);

    from = 0;
    code = 0;
    fromService = 0;
    codeService = 0;
    nextBlockSize = 0;
    serverCanal = new QCanal ("ServerCanal"); //канал, чтобы знать сколько сокетов
}

void Customer::run(){
    while (serverCanal->get() < 2);  //ждем диспетчера
    for (int counter = 0; counter < ORDER_COUNT; counter++) {
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

        if (code == Message::REJECTION){
            toFile("получил отказ");
            slotSend(Message::DISPATCHER, Message::READY);    //говорим диспетчеру что готовы
        }
        else if(code == Message::ORDER_COMPLETE){
            toFile("получил готовый заказ");

            slotSend(Message::COURIER, Message::MONEY_TRANSFER);    //передаем деньги
            toFile("передал деньги");
        }
    }
}

void Customer::slotReadyRead()
{
    QDataStream in(socket);
    in.setVersion(QDataStream::Qt_5_13);
    int to;
    in >> to >> from >> code;   //кому, от кого, код
}

//передаем на сервер, кому и что
void Customer::slotSend(int to, int code)
{
    QByteArray  arrBlock;
    QDataStream out(&arrBlock, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_13);
    out << to << Message::CUSTOMER << code;      //кому, от кого, код

    out.device()->seek(0);
    //out << quint16(arrBlock.size() - sizeof(quint16));

    socket->write(arrBlock);
}

void Customer::slotConnected()
{

}

void Customer::toFile(QString str)
{
    QFile File ("Customer.txt");
    while (!File.open(QFile::Append));
    QTextStream stream (&File);
    stream <<"Заказчик: " + str + "\n";
    File.close();
}

void Customer::readService()
{
    QDataStream in(socketService);
    in.setVersion(QDataStream::Qt_5_13);
    int to;
    in >> to >> fromService >> codeService;   //кому, от кого, код
}
