#ifndef GRPCCLIENT_H
#define GRPCCLIENT_H

#include <QObject>
#include <QTimer>
#include <QString>
#include "api.grpc.pb.h"


class GrpcClient : public QObject
{
    Q_OBJECT
public:
    explicit GrpcClient(const QString &serverAddress, QObject *parent = nullptr);
    ~GrpcClient();
    void connectToServer();
    void disconnectFromServer();

    bool isConnected() const;

signals:
    void pingSucceeded();
    void pingFailed();

private:
    void sendPing();

    QString serverAddress;
    std::unique_ptr<MaintainingApi::Stub> stub;
    QTimer pingTimer;
    int failedPings;

};

#endif // GRPCCLIENT_H
