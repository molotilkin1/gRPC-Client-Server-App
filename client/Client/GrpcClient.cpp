#include "GrpcClient.h"
#include <grpcpp/grpcpp.h>
#include <QDebug>


GrpcClient::GrpcClient(const QString &serverAddress, QObject *parent)
    : QObject(parent),
      serverAddress(serverAddress),
      stub(MaintainingApi::NewStub(grpc::CreateChannel(serverAddress.toStdString(), grpc::InsecureChannelCredentials()))),
     failedPings(0) {
    connect(&pingTimer, &QTimer::timeout, this, &GrpcClient::sendPing);
}

GrpcClient::~GrpcClient() {
    disconnectFromServer();
}

void GrpcClient::connectToServer() {
    if (isConnected()) return;

        pingTimer.start(5000); // 5 seconds interval
        qDebug() << "Connected to server:" << serverAddress;
}

void GrpcClient::disconnectFromServer() {
    pingTimer.stop();
    qDebug() << "Disconnected from server:" << serverAddress;
}

bool GrpcClient::isConnected() const {

    return pingTimer.isActive();
}

void GrpcClient::sendPing() {
    PingRequest request;
    PingResponse response;
    grpc::ClientContext context;

    request.set_clientip("192.168.0.1"); // Replace with the client's IP address 192.168.0.1
    auto status = stub->Ping(&context, request, &response);

    if (status.ok()) {
        failedPings = 0;
        emit pingSucceeded();
    } else {
        failedPings++;
        if (failedPings >= 3) {
            qDebug() << "Ping failed 3 times. Marking as Offline.";
            //disconnectFromServer();
            emit pingFailed();
        }
    }
}
