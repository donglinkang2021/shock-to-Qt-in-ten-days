#ifndef THREAD_H
#define THREAD_H

#include <QObject>
#include <QThread>
#include <QTcpSocket>
#include <QDebug>

class Thread : public QThread
{
    Q_OBJECT
public:
    explicit Thread(QTcpSocket *s);
    void run() override;

signals:
    void connectHostSignal(bool flag);

public slots:
    void connectHostSlot();

private:
    QTcpSocket *socket;


};

#endif // THREAD_H
