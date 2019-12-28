#include "server.h"

Server::Server(QObject *parent) : QObject(parent)//, firstSocket(NULL)
{
    server = new QTcpServer(this);
    WORKERS_COUNT = 10;
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
    if (sockets.count() < WORKERS_COUNT){
        sockets << socket;      //добавляем сокет
        mastersDetails << 0;    //добавляем слот для деталей
        forks << Message::FREE;   //добавляем свободную вилку
        // делаем обработчик изменения статуса сокета
        connect(sockets[sockets.count() - 1], SIGNAL(readyRead()), this, SLOT(readyRead())); // подключаем входящие сообщения от сокета на наш обработчик
        qDebug()<<"Подключен сокет "<<sockets.count() - 1;
        QByteArray arr;
        QDataStream in(&arr, QIODevice::WriteOnly);
        in << sockets.count() - 1;      //единственный передаваемый параметр - id
        in.device()->seek(0);
        sockets[sockets.count() - 1]->write(arr);   //пишем подключенному философу его id
    } else if (sockets.count() == WORKERS_COUNT) {
        storage = socket;       //это сокет склада
        QThread::msleep(450);  //небольшая пауза
        for (int i = 0; i < sockets.count(); i++) {
            QByteArray arrN;
            QDataStream in2(&arrN, QIODevice::WriteOnly);
            in2 << Message::START;
            in2.device()->seek(0);
            sockets[i]->write(arrN);   //говорим стартовать
        }
    }





    if (sockets.count() == 7){

    }

}
void Server::readyRead() // обработчик входящих сообщений
{
    QTcpSocket * socket = static_cast<QTcpSocket *>(QObject::sender()); // далее и ниже до цикла идет преобразования "отправителя сигнала" в сокет, дабы извлечь данные
    QByteArray arr =  socket->readAll();
    QDataStream in(arr);
    in.setVersion(QDataStream::Qt_5_11);
    int what, from, code;
    in >> what >> from >> code;   //что, от кого, код
    qDebug() << "Получили сообщение: " << what << " от: " << from << " код: " << code;

    QByteArray arrOut;
    QDataStream out(&arrOut, QIODevice::WriteOnly);

    switch (code) {
    case (Message::GET):{
        //запрос на левую вилку
        qDebug() << "Это GET";
        if (what == Message::LEFT){
            qDebug() << "Проверка соседа слева: его индекс " << abs((from + sockets.count() - 1) % sockets.count());
            if(masters.indexOf(abs((from + sockets.count() - 1) % sockets.count())) == -1)  //соседа слева еще нет в очереди, иначе он получит вилку как правую
            {  //masterStatus.value(abs((from + sockets.count() - 1) % sockets.count())) != Message::LEFT){

                qDebug() << "Мастер " << from << "просит инструмент в левую руку";
                //левая вилка свободна
                if (forks[from] == Message::FREE){
                    qDebug() << "Инструмент есть";
                    forks[from] = Message::USED;
                    qDebug() << "отдали инструмент " << from << " мастеру "<<from;
                    out << from << Message::LEFT << Message::GET;
                    out.device()->seek(0);
                    sockets[from]->write(arrOut);
                    masterStatus.insert(from, Message::LEFT);    //статус мастера - инструмент для левой руки
                }
                else{
                    qDebug() << "Инструмента нет";
                    masterStatus.insert(from, Message::NOT);    //статус мастера - нет инструмента
                }
            }
            else{
                qDebug() << "Инструмент достанется соседу слева";
                masterStatus.insert(from, Message::NOT);    //статус мастера - нет инструмента
            }
            masters << from;    //добавляем его в очередь
        }//просят в правую руку
        else if (what == Message::RIGHT) {
            qDebug() << "Мастер " << from << "просит инструмент в правую руку";
            //правая вилка свободна
            if (forks[(from + 1) % sockets.count()] == Message::FREE){
                qDebug() << "Инструмент есть";
                forks[(from + 1) % sockets.count()] = Message::USED;
                out << from << Message::RIGHT << Message::GET;
                qDebug() << "отдали инструмент " << (from + 1) % sockets.count() << " мастеру "<<from;
                out.device()->seek(0);
                sockets[from]->write(arrOut);
                masterStatus.insert(from, Message::RIGHT);    //статус мастера - инструмент для правой руки
                masters.removeAt(masters.indexOf(from));    //удаляем мастера из очереди - у него уже все есть
            }
            else{
                 masterStatus.insert(from, Message::RIGHT_WAIT);    //статус мастера - ЖДЕТ инструмент для правой руки
                qDebug() << "Инструмента нет";
            }
        }
        else
            qDebug() << "Get некорректен!";
        break;
    }

    case (Message::GIVE): {
        qDebug() << "Это GIVE";
        //мастер возвращает инструмент!

        //из левой руки
        if (what == Message::LEFT){
            forks[from] = Message::FREE;    //освобождаем инструмент
            qDebug() << from << "вернул инструмент " << from;
            masterStatus.insert(from, Message::NOT);    //статус мастера - пустые руки
        }
        //из правой руки
        else if (what == Message::RIGHT) {
            forks[(from + 1) % sockets.count()] = Message::FREE;    //освобождаем инструмент
            qDebug() << from << "вернул инструмент " << (from + 1) % sockets.count();
            masterStatus.insert(from, Message::LEFT);    //статус мастера - пустые руки
        }
        else
            qDebug() << "GIVE некорректен!";
        forkChanged();
        QByteArray arrComp;
        QDataStream outComp(&arrComp, QIODevice::WriteOnly);
        outComp << from << Message::COMPLETE;
        outComp.device()->seek(0);
        sockets[from]->write(arrComp);
        break;
    }
    case (Message::COMPLETE): {
        //мастер закончил изготовление!
        qDebug() << "зашли в COMPLETE ";
        ++mastersDetails[from]; //добавим его детальку к массиву
        qDebug() << "Детали: " << mastersDetails[0]<< mastersDetails[1]<<mastersDetails[2] << mastersDetails[3]<< mastersDetails[4];
        toFile("получена деталь от мастера " + QString::number(from));
        detailsChanged();
        qDebug() << "получена деталь от мастера " + QString::number(from);
        break;
    }
    default:{
        qDebug() << "??зашли в default??";
    }

    }
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
                (masters.indexOf(abs((master + sockets.count() - 1) % sockets.count())) > masters.indexOf(master) || (masters.indexOf(abs((master + sockets.count() - 1) % sockets.count())) == -1))){
                qDebug() << "fCh отдали инструмент " << master << " мастеру "<<master;
                forks[master] = Message::USED;
                out << master << Message::LEFT << Message::GET;
                sockets[master]->write(arrOut);
                masterStatus.insert(master, Message::LEFT);    //статус мастера - инструмент для левой руки

        }//просят в правую руку и правая вилка свободна
        else if (masterStatus.value(master) == Message::RIGHT_WAIT && forks[(master + 1) % sockets.count()] == Message::FREE) {
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
    qDebug() <<"зашли в detailsChanged";
    bool isComplect = true;
    foreach (int count, mastersDetails) {
        if (count == 0){
            isComplect = false;
            qDebug() <<"деталей недостаточно";
            break;
        }
    }
    if (isComplect){
        for (int i = 0; i < mastersDetails.count(); i++) {
            mastersDetails[i]--;
        }
        toFile("комплект сформирован и отправлен на склад\n");
        QByteArray arrOut;
        QDataStream out(&arrOut, QIODevice::WriteOnly);
        out << Message::COMPLETE;
        out.device()->seek(0);
        storage->write(arrOut);
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
