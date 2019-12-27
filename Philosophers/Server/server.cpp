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
    mastersDetails << 0;    //добавляем слот для деталей
    //connect(sockets[sockets.count() - 1], SIGNAL(stateChanged(QAbstractSocket::SocketState)), this, SLOT(stateChanged(QAbstractSocket::SocketState))); // делаем обработчик изменения статуса сокета
    connect(sockets[sockets.count() - 1], SIGNAL(readyRead()), this, SLOT(readyRead())); // подключаем входящие сообщения от сокета на наш обработчик
    //canal->put(sockets.count() - 1);    //число философов
    qDebug()<<"Подключен сокет "<<sockets[sockets.count() - 1];

    forks.append(Message::FREE);   //добавляем свободную вилку

    QByteArray arr;
    QDataStream in(&arr, QIODevice::WriteOnly);
    in << sockets.count() - 1;      //единственный передаваемый параметр - id
    sockets[sockets.count() - 1]->write(arr);   //пишем подключенному философу его id

    if (sockets.count() == 5)
        foreach (QTcpSocket * socketIt, sockets) {
            QByteArray arr;
            QDataStream in(&arr, QIODevice::WriteOnly);
            in << Message::START;
            socketIt->write(arr);   //говорим стартовать
        }

}
void Server::readyRead() // обработчик входящих сообщений
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
    QDataStream out(&arrOut, QIODevice::WriteOnly);

    switch (code) {
    case Message::GET:
        //запрос на левую вилку
        if (what == Message::LEFT && masters.indexOf(abs((from + sockets.count() - 1) % sockets.count()))){  //masterStatus.value(abs((from + sockets.count() - 1) % sockets.count())) != Message::LEFT){
            masters << from;    //добавляем
            qDebug() << "Мастер " << from << "просит инструмент в левую руку";
            //левая вилка свободна
            if (forks[from] == Message::FREE){
                qDebug() << "Инструмент есть";
                forks[from] = Message::USED;
                qDebug() << "отдали инструмент " << from << " мастеру "<<from;
                out << from << Message::LEFT << Message::GET;
                sockets[from]->write(arrOut);
                masterStatus.insert(from, Message::LEFT);    //статус мастера - инструмент для левой руки
            }
            else{
                qDebug() << "Инструмента нет";
                masterStatus.insert(from, Message::NOT);    //статус мастера - нет инструмента
            }
        }//просят в правую руку
        else if (what == Message::RIGHT) {
            qDebug() << "Мастер " << from << "просит инструмент в правую руку";
            //правая вилка свободна
            if (forks[(from + 1) % sockets.count()] == Message::FREE){
                qDebug() << "Инструмент есть";
                forks[(from + 1) % sockets.count()] = Message::USED;
                out << from << Message::RIGHT << Message::GET;
                qDebug() << "отдали инструмент " << (from + 1) % sockets.count() << " мастеру "<<from;
                sockets[from]->write(arrOut);
                masterStatus.insert(from, Message::RIGHT);    //статус мастера - инструмент для правой руки
                masters.removeAt(masters.indexOf(from));    //удаляем мастера из очереди - у него уже все есть
            }
        }
        else
            qDebug() << "Инструмента нет";
        break;

    case Message::GIVE: //мастер возвращает инструмент!
        qDebug() << "зашли в возврат ";
        //из левой руки
        if (what == Message::LEFT){
            forks[from] = Message::FREE;    //освобождаем инструмент
            qDebug() << from << "вернул инструмент " << from;
        }
        //из правой руки
        else if (what == Message::RIGHT) {
            forks[(from + 1) % sockets.count()] = Message::FREE;    //освобождаем инструмент
            qDebug() << from << "вернул инструмент " << (from + 1) % sockets.count();
            masterStatus.insert(from, Message::NOT);    //статус мастера - пустые руки
        }
        forkChanged();
        break;
    case Message::COMPLETE:     //мастер закончил изготовление!
        ++mastersDetails[from]; //добавим его детальку к массиву
        detailsChanged();
        toFile("получена деталь от мастера " + QString::number(from));
        break;


    }
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

//произошло изменение в вилках
void Server::forkChanged()
{
    qDebug() <<"зашли в forkChanged";
    foreach (int master, masters) {
        QByteArray arrOut;
        QDataStream out(&arrOut, QIODevice::WriteOnly);
        qDebug() <<"для " <<master<< " сосед " <<abs((master + sockets.count() - 1) % sockets.count());

        //мастер пуст, и левая вилка свободна
        if (masterStatus.value(master) == Message::NOT && forks[master] == Message::FREE &&
                (masters.indexOf(abs((master + sockets.count() - 1) % sockets.count())) > masters.indexOf(master) || masters.indexOf(abs((master + sockets.count() - 1) % sockets.count())) == -1)){
                qDebug() << "fCh отдали инструмент " << master << " мастеру "<<master;
                forks[master] = Message::USED;
                out << master << Message::LEFT << Message::GET;
                sockets[master]->write(arrOut);
                masterStatus.insert(master, Message::LEFT);    //статус мастера - инструмент для левой руки

        }//просят в правую руку и правая вилка свободна
        else if (masterStatus.value(master) == Message::LEFT && forks[(master + 1) % sockets.count()] == Message::FREE) {
            qDebug() << "fCh отдали инструмент " << (master + 1) % sockets.count() << " мастеру "<<master;
            forks[(master + 1) % sockets.count()] = Message::USED;
            out << master << Message::RIGHT << Message::GET;
            sockets[master]->write(arrOut);
            masterStatus.insert(master, Message::RIGHT);    //статус мастера - инструмент для правой руки
            masters.removeAt(masters.indexOf(master));    //удаляем мастера из очереди - у него уже все есть
        }
    }
}

void Server::detailsChanged()
{
    bool isComplect = true;
    foreach (int count, mastersDetails) {
        if (count == 0){
            isComplect = false;
            break;
        }
    }
    if (isComplect){
        for (int i = 0; i < mastersDetails.count(); i++) {
            --mastersDetails[i];
        }
        toFile("комплект сформирован и отправлен на склад\n");
    }
}

void Server::toFile(QString str)
{
    QFile File ("Dispetcher.txt");
    while (!File.open(QFile::Append));
    QTextStream stream (&File);
    stream <<"диспетчер: " + str + "\n";
    File.close();
}
