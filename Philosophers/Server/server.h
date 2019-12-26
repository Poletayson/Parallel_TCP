#ifndef SERVER_H
#define SERVER_H


#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QDebug>
#include <QByteArray>
#include <QDataStream>
#include <qcanal.h>
#include <message.h>

class Server : public QObject
{
    Q_OBJECT
public:
    explicit Server(QObject *parent = 0); // конструктор
    int getSocketsCount();
signals:
public slots:
    void incommingConnection(); // обработчик входящего подключения
    void readyRead(); // обработчик входящих данных
    void stateChanged(QAbstractSocket::SocketState stat); // обработчик изменения состояния вещающего сокета (он нам важен, дабы у нас всегда был кто-то, кто будет вещать
private:
    QTcpServer *server; // указатель на сервер
    QList<QTcpSocket *> sockets; // получатели данных
    QCanal *canal;
    QList <int> masters;  //очередь ожидающих мастеров
    QMap <int, int> masterStatus;   //статусы мастеров по айдишникам
    QList <int> forks;  //массив вилок
//    QTcpSocket *firstSocket; // вещатель

//    QList <QTcpSocket *> sockets;
//    QTcpSocket *customerSocket;
//    QTcpSocket *dispatcherSocket;
//    QTcpSocket *masterSocket;
//    QTcpSocket *storageSocket;
//    QTcpSocket *courierSocket;

    void forkChanged ();    //произошло изменение в вилках

};

#endif // SERVER_H
