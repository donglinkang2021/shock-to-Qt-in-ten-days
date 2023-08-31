#include "multhread.h"

mulThread::mulThread(QTcpSocket *s)
{
    socket = s;
}

void mulThread::run(){
    connect(
        socket,
        &QTcpSocket::readyRead,
        this,
        &mulThread::clientInfoSlot
    );
}

void mulThread::clientInfoSlot(){
    //    qDebug() << socket->readAll();
    QByteArray ba = socket->readAll();
    emit sendToWidget(ba);
}
