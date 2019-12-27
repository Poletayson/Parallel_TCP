#include "qcanal.h"

QCanal::QCanal():QSharedMemory()
{

}

QCanal::QCanal(QString key):QSharedMemory(key)
{
    isEmpty = true; //и
    if (create(2048));
    else
        attach();
}

bool QCanal::put(int val)
{
    lock();

    isEmpty = false;
    int *to = (int*)data();
    memcpy(to, &val, sizeof (int));

    unlock();
    return true;
}

int QCanal::get()
{
    return ((int*)data())[0];
}

void QCanal::lockCanal()
{

}

void QCanal::unlockCanal()
{
//    put(Message::EMPTY);
}

bool QCanal::getIsEmpty()
{
    attach();
    lock();
    void *dataPointer = data();
    int val = ((int*)dataPointer)[0];
    unlock();

//    return val == Message::EMPTY ? true : false;

}
