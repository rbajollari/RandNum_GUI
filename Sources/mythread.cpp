#include "mythread.h"
#include <QtCore>
#include <QtEndian>

MyThread::MyThread(QObject *parent) :
    QThread(parent)
{

}

void MyThread::run()
{
    socket = new QTcpSocket(this);
    socket->connectToHost("192.168.1.10", 7);

    QByteArray dataArray;
    unsigned int dataInt;
    qint64 maxlen = 4;

    while (1)
    {
        QMutex mutex;
        mutex.lock();
        if(this->Stop) break;
        mutex.unlock();

        if(socket->waitForConnected(3000))
        {
            socket->waitForBytesWritten(1000);
            socket->waitForReadyRead(3000);

            dataArray = socket->read(maxlen);

            dataInt = qFromLittleEndian<unsigned int>(dataArray.data());

            int length = dataArray.size();

            qDebug() << "Received" << dataInt << dataArray << length;
        }
        else
        {
            qDebug() << "Not connected!";
            break;
        }
    emit socketData(dataInt);
    }
}


