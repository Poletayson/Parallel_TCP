#ifndef SERVER_H
#define SERVER_H


#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QDebug>
#include <QByteArray>
#include <QDataStream>
#include <QFile>
#include <QThread>
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
private:
    QTcpServer *server; // указатель на сервер
    QList<QTcpSocket *> sockets; // получатели данных
    QCanal *canal;
    QList <int> masters;  //очередь ожидающих мастеров
    QList <int> mastersDetails;  //количество деталей, сделанных каждым мастером
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
    void detailsChanged ();    //произошло изменение в деталях
    void toFile (QString str);
};

#endif // SERVER_H
