// BruteWorker.h
#ifndef BRUTEWORKER_H
#define BRUTEWORKER_H

#include <QObject>
#include <QThread>
#include "PCANBasic.h"
#include <QMutex>


class BruteWorker : public QObject
{
    Q_OBJECT

public:
    explicit BruteWorker(QObject *parent = nullptr);
    ~BruteWorker();

    void startBrute();
    void stopBrute();

signals:
    void finished();

private:
    bool stopRequested;
    QMutex mutex;
    void bruteCANMessage(TPCANHandle channel, TPCANMsg& CANMsg);

};

#endif // BRUTEWORKER_H
