#ifndef SERVERWINDOW_H
#define SERVERWINDOW_H


#include <QMainWindow>
#include <QLineEdit>
#include <QTextEdit>
#include <QPushButton>
#include <QTimer>
#include <QUdpSocket>
#include <QDateTime>
#include <memory>
#include <grpcpp/grpcpp.h>
#include "api.grpc.pb.h"

QT_BEGIN_NAMESPACE
namespace Ui { class ServerWindow; }
QT_END_NAMESPACE

class ServerWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit ServerWindow(QWidget *parent = nullptr);
    ~ServerWindow();

    void logEvent(const QString &message);

private slots:
    void startServer();
    void stopServer();
    void handleBroadcast();
    void checkPingTimeout();

private:
    // UI Elements
    QLineEdit *portLineEdit;
    QTextEdit *logTextEdit;
    QPushButton *startButton;

    // Network and timers
    QUdpSocket *udpSocket;
    QTimer *broadcastTimer;
    QTimer *pingTimer;

    // gRPC Server
    std::unique_ptr<grpc::Server> grpcServer;

    // State management
    bool isClientConnected;
    QDateTime lastPingTime;

    // Internal methods
    void setupUi();
    void stopBroadcast();

    // gRPC Service Implementation
    class MaintainingServiceImpl : public MaintainingApi::Service
    {
    public:
        explicit MaintainingServiceImpl(ServerWindow *parent);

        grpc::Status Ping(grpc::ServerContext *context,
                          const PingRequest *request,
                          PingResponse *response) override;

    private:
        ServerWindow *parent;
    };

    std::unique_ptr<MaintainingServiceImpl> maintainingService;
};
#endif // SERVERWINDOW_H
