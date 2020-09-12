#ifndef WORKER_H
#define WORKER_H

#include <QObject>

class Worker : public QObject
{
    Q_OBJECT
public:
    explicit Worker(QObject *parent = nullptr);

public slots:
    void process();

signals:
    void finished();
    void progressUpdate(int value);

public slots:
};

#endif // WORKER_H
