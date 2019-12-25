#include "master.h"

#include <QHostAddress>

Master::Master(QObject *parent) : QObject(parent)
{

    from = 0;
    code = 0;
    nextBlockSize = 0;

}


void Master::run()
{
    socket = new QTcpSocket();
    socket->connectToHost(QHostAddress("127.0.0.1"), 2140);
    connect(socket, SIGNAL(connected()), SLOT(slotConnected()));
    //connect(socket, SIGNAL(readyRead()), SLOT(slotReadyRead()));
    socketService = new QTcpSocket();
    socketService->connectToHost("localhost", 2141);
    serverCanal = new QCanal ("ServerCanal"); //канал, чтобы знать сколько сокетов

    while (serverCanal->get() < 5);  //ждем склад и курьера
    //slotSend(Message::DISPATCHER, Message::READY);    //говорим диспетчеру что готовы
    qDebug() << "Все в сборе, начинаем";
    while (true) {
        //пока в канал мастера не поступит заказ
        socket->waitForReadyRead(); //ждем когда ответят
        slotReadyRead();    //читаем что нам прислали
        QThread::msleep(Message::DELAY);
        qDebug() << "Получили от: " << from <<  " код: " << code;
        if (code == Message::MAKE_ORDER){
            toFile("получил заказ");
            slotSend(Message::STORAGE, Message::MATERIALS_REQUEST);    //запрашиваем у склада
            toFile("запросил материалы");
            qDebug() << "Запросили материалы";
            //ждем отказ или материалы
            socket->waitForReadyRead(); //ждем когда ответят
            slotReadyRead();    //читаем что нам прислали
            //while (masterCanal->get().getType() != Message::MATERIALS_ARE && masterCanal->get().getType() != Message::MATERIALS_ARE_NOT);
            QThread::msleep(Message::DELAY);
            qDebug() << "Ответ от склада: " << from <<  " код: " << code;
            //материалы поступили
            if (code == Message::MATERIALS_ARE){
                slotSend(Message::COURIER, Message::ORDER_COMPLETE);    //заказ готов, курьер должен забрать
                toFile("передал заказ курьеру");
            }
            else {
                slotSend(Message::DISPATCHER, Message::REJECTION);    //передаем отказ
                toFile("передал отказ");
            }
        }
        else{
            qDebug() << "Что-то не то!";
            toFile("Что-то не то: ");
        }
    }

}

void Master::toFile(QString str)
{
    QFile File ("Master.txt");
    while (!File.open(QFile::Append));
    QTextStream stream (&File);
    stream <<"Мастер: " + str + "\n";
    File.close();
}

void Master::slotReadyRead()
{
    QDataStream in(socket);
    in.setVersion(QDataStream::Qt_5_11);
    int to;
    in >> to >> from >> code;   //кому, от кого, код
}

//передаем на сервер, кому и что
void Master::slotSend(int to, int code)
{
    QByteArray  arrBlock;
    QDataStream out(&arrBlock, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_11);
    out << to << Message::DISPATCHER << code;      //кому, от кого, код

    out.device()->seek(0);
    //out << quint16(arrBlock.size() - sizeof(quint16));

    socket->write(arrBlock);
}

void Master::slotConnected()
{

}
