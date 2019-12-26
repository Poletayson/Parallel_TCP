#include "server.h"

Server::Server(QObject *parent) : QObject(parent)//, firstSocket(NULL)
{
    server = new QTcpServer(this);
    canal = new QCanal ("ServerCanal"); //канал, чтобы знать сколько сокетов
    canal->put(0);
    qDebug() << "server listen = " << server->listen(QHostAddress("127.0.0.1"), 2140);
    connect(server, SIGNAL(newConnection()), this, SLOT(incommingConnection())); // подключаем сигнал "новое подключение" к нашему обработчику подключений
}

int Server::getSocketsCount()
{
    return sockets.count();
}

void Server::incommingConnection() // обработчик подключений
{
    QTcpSocket * socket = server->nextPendingConnection();  //получаем сокет нового входящего подключения
    sockets << socket;      //добавляем сокет
    //connect(sockets[sockets.count() - 1], SIGNAL(stateChanged(QAbstractSocket::SocketState)), this, SLOT(stateChanged(QAbstractSocket::SocketState))); // делаем обработчик изменения статуса сокета
    connect(sockets[sockets.count() - 1], SIGNAL(readyRead()), this, SLOT(readyRead())); // подключаем входящие сообщения от сокета на наш обработчик
    //canal->put(sockets.count() - 1);    //число философов
    qDebug()<<"Подключен сокет "<<canal->get();

    forks.append(Message::FREE);   //добавляем свободную вилку

    QByteArray arr;
    QDataStream in(arr);
    in << sockets.count() - 1;      //единственный передаваемый параметр - id

    sockets[sockets.count() - 1]->write(arr);   //пишем подключенному философу его id

}
void Server::readyRead() // обработчик входящих сообщений от "вещающего"
{
    qDebug() << "Получили сообщение";
    QTcpSocket * socket = static_cast<QTcpSocket *>(QObject::sender()); // далее и ниже до цикла идет преобразования "отправителя сигнала" в сокет, дабы извлечь данные
    QByteArray arr =  socket->readAll();
    QDataStream in(arr);
    in.setVersion(QDataStream::Qt_5_11);
    int what, from, code;
    in >> what >> from >> code;   //что, от кого, код
    qDebug() << "Серверу пришло сообщение: " << what << " " << from << " " << code;

    QByteArray arrOut;
    QDataStream out(arrOut);

    switch (code) {
    case Message::GET:
        //запрос на левую вилку
        if (what == Message::LEFT){
            masters << from;    //добавляем
            //левая вилка свободна
            if (forks[from] == Message::FREE){
                forks[from] = Message::USED;
                out << from << Message::LEFT << Message::GET;
                sockets[from]->write(arrOut);
                masterStatus.insert(from, Message::LEFT);    //статус мастера - инструмент для левой руки
            }
            else
                masterStatus.insert(from, Message::NOT);    //статус мастера - нет инструмента
        }//просят в правую руку
        else if (what == Message::RIGHT) {

            //правая вилка свободна
            if (forks[(from + 1) % sockets.count()] == Message::FREE){
                forks[(from + 1) % sockets.count()] = Message::USED;
                out << from << Message::RIGHT << Message::GET;
                sockets[from]->write(arrOut);
                masterStatus.insert(from, Message::RIGHT);    //статус мастера - инструмент для левой руки
                masters.removeAt(masters.indexOf(from));    //удаляем мастера из очереди - у него уже все есть
            }
        }
        break;

    case Message::GIVE: //мастер возвращает инструмент!
        //из левой руки
        if (what == Message::LEFT){
            forks[from] = Message::FREE;    //освобождаем инструмент
        }
        //из правой руки
        else if (what == Message::RIGHT) {
            forks[(from + 1) % sockets.count()] = Message::FREE;    //освобождаем инструмент
            masterStatus.insert(from, Message::NOT);    //статус мастера - пустые руки
        }
        break;
    case Message::COMPLETE:     //мастер закончил изготовление!
//здесь нужно добавить его детальку к массиву
        break;


    }


//    if (code == Message::GET)   else if (code == Message::GIVE)



    //sockets[to]->write(arr);
//


//    foreach(QTcpSocket *socket, sockets) { // пишем входящие данные от "вещающего" получателям
//        if (socket->state() == QTcpSocket::ConnectedState)
//            socket->write(arr);
//    }
}
void Server::stateChanged(QAbstractSocket::SocketState state) // обработчик статуса, нужен для контроля за "вещающим"
{
//    QObject * object = QObject::sender();
//    if (!object)
//        return;
//    QTcpSocket * socket = static_cast<QTcpSocket *>(object);
//    qDebug() << state;
//    if (socket == firstSocket && state == QAbstractSocket::UnconnectedState)
    //        firstSocket = NULL;
}

void Server::forkChanged()
{

}
