#ifndef MYTHREAD_H
#define MYTHREAD_H

#include <QObject>

class myThread : public QObject
{
    Q_OBJECT
public:
    explicit myThread(QObject *parent = nullptr);

signals:

};

#endif // MYTHREAD_H
