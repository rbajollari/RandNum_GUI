#ifndef MYTHREAD_H
#define MYTHREAD_H

#include <QThread>
#include <QTimer>
#include <QTcpSocket>
#include <QDebug>

class MyThread : public QThread
{
    Q_OBJECT
public:
    explicit MyThread(QObject *parent = 0);
    void run();
    bool Stop = false;

signals:
    void socketData(int);

public slots:

private:
    QTimer dataTimer;
    QTcpSocket *socket;

};

#endif // MYTHREAD_H
