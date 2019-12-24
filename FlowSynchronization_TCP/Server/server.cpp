#include "server.h"

Server::Server(QObject *parent) : QObject(parent)//, firstSocket(NULL)
{
    server = new QTcpServer(this);
    canal = new QCanal ("ServerCanal"); //канал, чтобы знать сколько сокетов
    canal->put(0);
    qDebug() << "server listen = " << server->listen(QHostAddress::Any, 2140);
    connect(server, SIGNAL(newConnection()), this, SLOT(incommingConnection())); // подключаем сигнал "новое подключение" к нашему обработчику подключений
}

int Server::getSocketsCount()
{
    return sockets.count();
}

void Server::incommingConnection() // обработчик подключений
{
    QTcpSocket * socket = server->nextPendingConnection(); // получаем сокет нового входящего подключения
    connect(socket, SIGNAL(stateChanged(QAbstractSocket::SocketState)), this, SLOT(stateChanged(QAbstractSocket::SocketState))); // делаем обработчик изменения статуса сокета
    connect(socket, SIGNAL(readyRead()), this, SLOT(readyRead())); // подключаем входящие сообщения от сокета на наш обработчик
    sockets << socket;      //добавляем сокет
    canal->put(sockets.count());


//    if (!firstSocket) { // если у нас нет "вещающего", то данное подключение становится вещающим
//        connect(socket, SIGNAL(readyRead()), this, SLOT(readyRead())); // подключаем входящие сообщения от вещающего на наш обработчик
//        socket->write("server"); // говорим ему что он "вещает"
//        firstSocket = socket; // сохраняем себе"
//        qDebug() << "this one is server";
//    }
//    else { // иначе говорим подключенному что он "получатель"
//        socket->write("client");
//        sockets << socket;
//    }
}
void Server::readyRead() // обработчик входящих сообщений от "вещающего"
{
    QTcpSocket * socket = static_cast<QTcpSocket *>(QObject::sender()); // далее и ниже до цикла идет преобразования "отправителя сигнала" в сокет, дабы извлечь данные
    QByteArray arr =  socket->readAll();
    QDataStream in(arr);
    in.setVersion(QDataStream::Qt_5_13);
    int to, from, code;
    in >> to >> from >> code;   //кому, от кого, код

    sockets[to]->write(arr);
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
