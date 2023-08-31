#include "thread.h"

Thread::Thread(QTcpSocket *s)
{
    socket = s;
}

void Thread::run(){
    connect(
        socket,
        &QTcpSocket::readyRead,
        this,
        &Thread::connectHostSlot
    );
}

void Thread::connectHostSlot(){
    emit connectHostSignal(
        socket->waitForConnected(1000)
    );
}
