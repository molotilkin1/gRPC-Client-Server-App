#include "ServerWindow.h"
#include "ui_ServerWindow.h"
#include <QVBoxLayout>
#include <QDateTime>
#include <QHostAddress>

ServerWindow::ServerWindow(QWidget *parent)
    : QMainWindow(parent),
      udpSocket(new QUdpSocket(this)),
      broadcastTimer(new QTimer(this)),
      pingTimer(new QTimer(this)),
      isClientConnected(false)
{
    setupUi();

    connect(startButton, &QPushButton::clicked, this, &ServerWindow::startServer);
    connect(broadcastTimer, &QTimer::timeout, this, &ServerWindow::handleBroadcast);
    connect(pingTimer, &QTimer::timeout, this, &ServerWindow::checkPingTimeout);
   this->resize(650,400);
}
ServerWindow::MaintainingServiceImpl::MaintainingServiceImpl(ServerWindow *parent)
    : parent(parent)
{
}

ServerWindow::~ServerWindow()
{
    stopBroadcast();
    if (grpcServer) {
        grpcServer->Shutdown();
    }
}

void ServerWindow::setupUi()
{
    auto *centralWidget = new QWidget(this);
    auto *layout = new QVBoxLayout(centralWidget);

    portLineEdit = new QLineEdit(this);
    portLineEdit->setPlaceholderText("Введите порт сервера (default: 10001)");
    layout->addWidget(portLineEdit);

    logTextEdit = new QTextEdit(this);
    logTextEdit->setReadOnly(true);
    layout->addWidget(logTextEdit);

    startButton = new QPushButton("Запуск", this);
    layout->addWidget(startButton);

    setCentralWidget(centralWidget);
    setWindowTitle("gRPC Сервер");


}

void ServerWindow::logEvent(const QString &message)
{
    logTextEdit->append(QDateTime::currentDateTime().toString("[hh:mm:ss] ") + message);
}

void ServerWindow::startServer()
{
    // Проверяем текущий текст кнопки
    if (startButton->text() == "Остановить") {
        stopServer();
        return; // Завершаем выполнение метода
    }

    bool ok;
    int port = portLineEdit->text().toInt(&ok);
    if (!ok || port <= 0 || port > 65535) {
        logEvent("Invalid port specified. Please enter a valid port.");
        return;
    }

    logEvent("Запуск gRPC сервера. Порт: " + QString::number(port));

    // Запуск gRPC сервера
    maintainingService = std::unique_ptr<ServerWindow::MaintainingServiceImpl>(new ServerWindow::MaintainingServiceImpl(this));

    grpc::ServerBuilder builder;
    builder.AddListeningPort("0.0.0.0:" + std::to_string(port), grpc::InsecureServerCredentials());
    builder.RegisterService(maintainingService.get());
    grpcServer = builder.BuildAndStart();
    logEvent("gRPC сервер запущен успешно.");

    // Запуск UDP Broadcast
    broadcastTimer->start(1000); // Интервал 1 секунда
    logEvent("IP и порт сервера вещания...");

    startButton->setText("Остановить");
}

void ServerWindow::stopServer()
{
    // Остановка Broadcast
    stopBroadcast();

    // Завершение gRPC сервера
    if (grpcServer) {
        grpcServer->Shutdown();
        grpcServer.reset();
        logEvent("gRPC сервер остановлен.");
    }

    // Обновление кнопки
    startButton->setText("Запуск");
}

void ServerWindow::handleBroadcast()
{
    if (isClientConnected) return;

    QByteArray message = QString("IP:%1 PORT:%2")
                             .arg(QHostAddress(QHostAddress::LocalHost).toString())
                             .arg(portLineEdit->text())
                             .toUtf8();

    udpSocket->writeDatagram(message, QHostAddress::Broadcast, 10001);
    logEvent("Отправлено широковещательное сообщение: " + QString(message));
}

void ServerWindow::stopBroadcast()
{
    broadcastTimer->stop();
    logEvent("Трансляция завершена.");
}

grpc::Status ServerWindow::MaintainingServiceImpl::Ping(grpc::ServerContext *context, const PingRequest *request, PingResponse *response)
{
    Q_UNUSED(context);
    Q_UNUSED(response);

    QString clientIp = QString::fromStdString(request->clientip());
    parent->logEvent("Ping received from: " + clientIp);
    parent->isClientConnected = true;
    parent->lastPingTime = QDateTime::currentDateTime();

    return grpc::Status::OK;
}

void ServerWindow::checkPingTimeout()
{
    if (!isClientConnected) return;

    if (lastPingTime.msecsTo(QDateTime::currentDateTime()) > 15000) { // 15 секунд
        logEvent("Ping timeout. Resuming broadcast...");
        isClientConnected = false;
        broadcastTimer->start(1000);
    }
}
