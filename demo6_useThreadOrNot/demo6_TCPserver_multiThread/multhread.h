#ifndef MULTHREAD_H
#define MULTHREAD_H

#include <QObject>
#include <QThread>
#include <QTcpSocket>
#include <QDebug>

class mulThread :public QThread
{
    Q_OBJECT
public:
    explicit mulThread(QTcpSocket *s);
    void run() override;

signals:
    void sendToWidget(QByteArray ba);

public slots:
    void clientInfoSlot();

private:
    QTcpSocket *socket;

};

#endif // MULTHREAD_H
