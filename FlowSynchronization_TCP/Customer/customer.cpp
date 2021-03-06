#include "customer.h"

#include <QHostAddress>



Customer::Customer(QObject *parent) : QObject(parent)
{

    from = 0;
    code = 0;
    fromService = 0;
    codeService = 0;
    nextBlockSize = 0;
}

void Customer::run(){
    socket = new QTcpSocket();
    socket->connectToHost(QHostAddress("127.0.0.1"), 2140);
    connect(socket, SIGNAL(connected()), SLOT(slotConnected()));
    //connect(socket, SIGNAL(readyRead()), SLOT(slotReadyRead()));

    socketService = new QTcpSocket();
    socketService->connectToHost("localhost", 2141);


    serverCanal = new QCanal ("ServerCanal"); //канал, чтобы знать сколько сокетов


    while (serverCanal->get() < 2);  //ждем диспетчера
    qDebug()<<"Диспетчер запущен";
    for (int counter = 0; counter < ORDER_COUNT; counter++) {
        QThread::msleep(Message::DELAY);    //небольшая пауза
        while (from != Message::DISPATCHER || code != Message::READY){    //пока диспетчер не освободится
            qDebug()<<"Начинаем ждать диспетчера";
            socket->waitForReadyRead(); //ждем пока диспетчер не будет готов
            slotReadyRead();    //читаем что нам прислали
        }

        qDebug()<<"Сейчас закажем";
        slotSend(Message::DISPATCHER, Message::MAKE_ORDER); //делаем заказ
        toFile("заказал стул");
        qDebug()<<"Заказали, ждем ответ";

        socket->waitForReadyRead(); //ждем когда ответят
        QThread::msleep(Message::DELAY);//небольшая пауза
        slotReadyRead();    //читаем что нам прислали

        qDebug() << "Получили ответ от: " << from <<  " код: " << code;
        if (code == Message::REJECTION){
            toFile("получил отказ\n");
            qDebug() << "ОТКАЗ";
            slotSend(Message::DISPATCHER, Message::READY);    //говорим диспетчеру что готовы

            qDebug()<<"Можно начинать заново";

        }
        else if(code == Message::ORDER_COMPLETE){
            toFile("получил готовый заказ");

            slotSend(Message::COURIER, Message::MONEY_TRANSFER);    //передаем деньги
            toFile("передал деньги\n");

            qDebug()<<"Можно начинать заново";
        }
    }
}

void Customer::slotReadyRead()
{
    QDataStream in(socket);
    in.setVersion(QDataStream::Qt_5_11);
    int to;
    in >> to >> from >> code;   //кому, от кого, код
}

//передаем на сервер, кому и что
void Customer::slotSend(int to, int code)
{
    QByteArray  arrBlock;
    QDataStream out(&arrBlock, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_11);
    out << to << Message::CUSTOMER << code;      //кому, от кого, код

    out.device()->seek(0);
    //out << quint16(arrBlock.size() - sizeof(quint16));

    socket->write(arrBlock);
    //emit socket->bytesWritten(12);
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
    in.setVersion(QDataStream::Qt_5_11);
    int to;
    in >> to >> fromService >> codeService;   //кому, от кого, код
}
