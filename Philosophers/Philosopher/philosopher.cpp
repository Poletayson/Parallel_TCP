#include "philosopher.h"

#include <QHostAddress>

Philosopher::Philosopher(QObject *parent) : QObject(parent)
{

//    from = 0;
//    code = 0;
//    nextBlockSize = 0;

}


void Philosopher::run()
{
    socket = new QTcpSocket();

    socket->connectToHost(QHostAddress("127.0.0.1"), 2140);
    socket->waitForReadyRead(); //ждем когда нам пришлют наш id
    QDataStream idStream(socket);
    idStream >> id;     //получаем наш id
    socket->waitForReadyRead(); //ждем когда нам пришлют наш id
    int start;
    idStream >> start;     //получаем сигнал

    //serverCanal = new QCanal ("ServerCanal"); //канал, чтобы знать сколько сокетов

    if(start == Message::START)
    {
        while (true) {
            int waitTime = qrand() % Message::DELAY_MAX;
            toFile("решил отдохнуть " + QString::number(waitTime) + "мс8 ");
            QThread::msleep(waitTime);  //небольшая пауза
            slotSend(Message::LEFT, Message::GET);     //просим инструмент в левую руку
            toFile("запросил инструмент в левую руку");
            socket->waitForReadyRead(); //ждем когда ответят
            slotReadyRead();    //читаем что нам прислали
            if (first == Message::LEFT && second == Message::GET){
                //получили!
                toFile("получил инструмент в левую руку");
                slotSend(Message::RIGHT, Message::GET);     //просим инструмент в правую руку
                toFile("запросил инструмент в правую руку");
                socket->waitForReadyRead(); //ждем когда ответят
                slotReadyRead();    //читаем что нам прислали
                if (first == Message::RIGHT && second == Message::GET){
                    toFile("получил инструмент в правую руку");
                    QThread::msleep(qrand() % Message::DELAY_MAX);  //небольшая пауза
                    slotSend(id, Message::COMPLETE);     //деталь готова
                    toFile("передал свою деталь");

                    slotSend(Message::LEFT, Message::GIVE);     //возвращаем инструмент из левой руки
                    toFile("вернул инструмент из левой руки");
                    slotSend(Message::RIGHT, Message::GIVE);     //возвращаем инструмент из правой руки
                    toFile("вернул инструмент из правой руки\n");
                }
                else {
                    toFile("ЧТО-ТО НЕ ТАК! Ждал инструмент в правую руку");
                }
            }
            else {
                toFile("ЧТО-ТО НЕ ТАК! Ждал инструмент в левую руку");
            }
        }
    }

}

void Philosopher::toFile(QString str)
{
    QFile File ("Master_" + QString::number(id) + ".txt");
    while (!File.open(QFile::Append));
    QTextStream stream (&File);
    stream <<"мастер: " + str + "\n";
    File.close();
}

void Philosopher::slotReadyRead()
{
    QDataStream in(socket);
    in.setVersion(QDataStream::Qt_5_11);
    int to;
    in >> to >> first >> second;   //кому, от кого, код
}

//передаем на сервер, кому и что
void Philosopher::slotSend(int forkNum, int action)
{
    QByteArray  arrBlock;
    QDataStream out(&arrBlock, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_11);
    out << forkNum << id << action;      //какая вилка от кого, и что делать

    out.device()->seek(0);

    socket->write(arrBlock);
}

void Philosopher::slotConnected()
{

}
