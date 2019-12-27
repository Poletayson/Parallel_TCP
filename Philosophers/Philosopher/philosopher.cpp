#include "philosopher.h"

#include <QHostAddress>

Philosopher::Philosopher(QObject *parent) : QObject(parent)
{
    semaphoreServer = new QSystemSemaphore ("semaphoreServer");
    semaphorePhilosopher = new QSystemSemaphore ("semaphorePhilosopher");

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
            toFile("решил отдохнуть " + QString::number(waitTime) + "мс");
            QThread::msleep(waitTime);  //небольшая пауза
            slotSend(Message::LEFT, Message::GET);     //просим инструмент в левую руку
            toFile("запросил инструмент в левую руку");
            do{
                socket->waitForReadyRead(INT_MAX); //ждем когда ответят
                slotReadyRead();    //читаем что нам прислали
            } while (first != Message::LEFT);   //ждем

            if (first == Message::LEFT && second == Message::GET){
                //получили!
                toFile("получил инструмент в левую руку");
                slotSend(Message::RIGHT, Message::GET);     //просим инструмент в правую руку
                toFile("запросил инструмент в правую руку");
                do{
                    socket->waitForReadyRead(INT_MAX); //ждем когда ответят
                    slotReadyRead();    //читаем что нам прислали
                }while (first != Message::RIGHT);   //ждем
                //if (first == Message::RIGHT && second == Message::GET){
                    toFile("получил инструмент в правую руку");
                    QThread::msleep(qrand() % Message::DELAY_MAX);  //небольшая пауза

                    slotSend(Message::RIGHT, Message::GIVE);     //возвращаем инструмент из правой руки
                    toFile("вернул инструмент из правой руки\n");
                    //QThread::msleep(qrand() % Message::DELAY_MAX + 350);  //небольшая пауза

                    do{
                        socket->waitForReadyRead(INT_MAX); //ждем когда ответят
                        slotReadyRead();    //читаем что нам прислали
                    }while (first != Message::COMPLETE);   //ждем пока сервер не примет


                    slotSend(Message::LEFT, Message::GIVE);     //возвращаем инструмент из левой руки
                    toFile("вернул инструмент из левой руки");

                    do{
                        socket->waitForReadyRead(INT_MAX); //ждем когда ответят
                        slotReadyRead();    //читаем что нам прислали
                    }while (first != Message::COMPLETE);   //ждем пока сервер не примет

                    slotSend(-1, Message::COMPLETE);     //деталь готова
                    toFile("передал свою деталь");


                //}
//                else {
//                    toFile("ЧТО-ТО НЕ ТАК! Ждал инструмент в правую руку\n");
//                }
            }
            else {
                toFile("ЧТО-ТО НЕ ТАК! Ждал инструмент в левую руку\n");
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
void Philosopher::slotSend(int what, int code)
{
    QByteArray  arrBlock;
    QDataStream out(&arrBlock, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_11);
    out << what << id << code;   //что, от кого, код

    out.device()->seek(0);

    socket->write(arrBlock);
}

void Philosopher::slotConnected()
{

}
