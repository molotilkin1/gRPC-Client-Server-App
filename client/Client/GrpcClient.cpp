#include "GrpcClient.h"
#include <grpcpp/grpcpp.h>
#include <QDebug>


GrpcClient::GrpcClient(const QString &serverAddress, QObject *parent)
    : QObject(parent),
      serverAddress(convertIPv6ToIPv4(serverAddress)),
      stub(MaintainingApi::NewStub(grpc::CreateChannel(this->serverAddress.toStdString(), grpc::InsecureChannelCredentials()))),
      failedPings(0) {
    connect(&pingTimer, &QTimer::timeout, this, &GrpcClient::sendPing);
    qDebug() << "Creating gRPC channel to:" << this->serverAddress;
}

GrpcClient::~GrpcClient() {
    disconnectFromServer();
}

QString GrpcClient::convertIPv6ToIPv4(const QString &address) {
    if (address.startsWith("::ffff:")) {
        return address.mid(7); // Убираем "::ffff:"
    }
    return address;
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

    request.set_clientip("192.168.0.1"); // Replace with the client's IP address

    // Устанавливаем тайм-аут
    /*std::chrono::system_clock::time_point deadline = std::chrono::system_clock::now() + std::chrono::seconds(2);
    context.set_deadline(deadline);*/

    qDebug() << "Sending Ping to server with IP:" << serverAddress;

    auto status = stub->Ping(&context, request, &response);

    if (status.ok()) {
        qDebug() << "Ping succeeded. Server response:" << QString::fromStdString(response.DebugString());
        failedPings = 0;
        emit pingSucceeded();
    } else {
        qDebug() << "Ping failed with error:" << QString::fromStdString(status.error_message());
        failedPings++;
        if (failedPings >= 3) {
            qDebug() << "Ping failed 3 times. Marking as Offline.";
            emit pingFailed();
        }
    }
}
