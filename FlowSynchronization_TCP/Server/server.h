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

private:
    QTcpServer *server; // указатель на сервер
    QList<QTcpSocket *> sockets; // получатели данных
    QCanal *canal;

};

#endif // SERVER_H
