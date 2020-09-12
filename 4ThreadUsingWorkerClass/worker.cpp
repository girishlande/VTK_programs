#include "worker.h"
#include "qdebug.h"
#include "qthread.h"
Worker::Worker(QObject *parent) : QObject(parent)
{

}

void Worker::process()
{
    qDebug("Worker process started!");

    for(int i=1;i<=100;i++) {
        QThread::msleep(100);
        emit progressUpdate(i);
    }

    qDebug("Worker process finished!");
    emit finished();
}
