#include "MainWindow.h"
#include "ui_MainWindow.h"
#include <QHeaderView>
#include <QMessageBox>
#include <QDateTime>


MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent),
      centralWidget(new QWidget(this)),
      tableView(new QTableView(this)),
      startDiscoveryButton(new QPushButton("Поиск серверов", this)),
      layout(new QVBoxLayout(centralWidget)) {

    // Устанавливаем модель для таблицы
    tableView->setModel(&serverTableModel);
    tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    // Добавляем элементы интерфейса в макет
    layout->addWidget(tableView);
    layout->addWidget(startDiscoveryButton);
    setCentralWidget(centralWidget);

    // Подключаем сигналы и слоты
    connect(startDiscoveryButton, &QPushButton::clicked, this, &MainWindow::onStartDiscovery);
    connect(&udpService, &UdpDiscoveryService::serverDiscovered, this, &MainWindow::onServerDiscovered);

    // Устанавливаем коллбек для кнопок в таблице
    connect(tableView, &QTableView::clicked, [this](const QModelIndex& index) {
        if (index.column() == 3) { // "Action" колонка
            onActionButtonClicked(index.row());
        }
    });

    setWindowTitle("gRPC Client");
    resize(800, 600);
}

void MainWindow::onStartDiscovery() {
    udpService.startDiscovery();
    startDiscoveryButton->setEnabled(false);
    QMessageBox::information(this, "Поиск", "Поиск начался. Ожидание серверов...");
}

void MainWindow::onServerDiscovered(const QString& address) {
    serverTableModel.addServer(address);
}

void MainWindow::onActionButtonClicked(int row) {
    /*auto index = serverTableModel.index(row, 0); // Адрес сервера
    QString serverAddress = serverTableModel.data(index).toString();

    auto statusIndex = serverTableModel.index(row, 2); // Статус сервера
    QString status = serverTableModel.data(statusIndex).toString();*/

    QString status = serverTableModel.data(serverTableModel.index(row, 2), Qt::DisplayRole).toString();
    QString serverAddress = serverTableModel.data(serverTableModel.index(row, 0), Qt::DisplayRole).toString();

    GrpcClient* client = new GrpcClient(serverAddress, this);

    if (status == "Offline") {
        // Подключаемся к серверу

        client->connectToServer();
        // Успешный пинг
        connect(client, &GrpcClient::pingSucceeded, [this, row]() {
            auto timeIndex = serverTableModel.index(row, 1);
            serverTableModel.setData(timeIndex, QTime::currentTime().toString("HH:mm:ss"));

            auto statusIndex = serverTableModel.index(row, 2);
            serverTableModel.setData(statusIndex, "Online");

            auto actionIndex = serverTableModel.index(row, 3);
            serverTableModel.setData(actionIndex, "Disconnect");


            QString currentTime = QTime::currentTime().toString("HH:mm:ss");

            // Обновляем время пинга
            serverTableModel.updateLastPingTime(row, currentTime);

            // Обновляем статус на "Online"
            serverTableModel.updateStatus(row, "Online");
        });

        // Пинг неудачен
        connect(client, &GrpcClient::pingFailed, [this, row, client]() {
            auto statusIndex = serverTableModel.index(row, 2);
            serverTableModel.setData(statusIndex, "Offline");

            auto actionIndex = serverTableModel.index(row, 3);
            serverTableModel.setData(actionIndex, "Connect");

            //client->deleteLater();
            // Обновляем статус на "Offline"
            serverTableModel.updateStatus(row, "Offline");
            client->deleteLater();
        });



        //serverTableModel.setData(statusIndex, "Online");
    } else if (status == "Online") {
        // Отключаемся от сервера
        /*auto statusIndex = serverTableModel.index(row, 2);
        serverTableModel.setData(statusIndex, "Offline");*/

        // Отключаемся от сервера
        client->disconnectFromServer();
        serverTableModel.updateStatus(row, "Offline");

        auto actionIndex = serverTableModel.index(row, 3);
        serverTableModel.setData(actionIndex, "Connect");

        client->deleteLater();
    }
    emit serverTableModel.dataChanged(serverTableModel.index(row, 3), serverTableModel.index(row, 3));
}
