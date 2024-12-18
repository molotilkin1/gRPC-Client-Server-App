#include "UdpDiscoveryService.h"
#include <QDebug>
#include <QHostAddress>

UdpDiscoveryService::UdpDiscoveryService(QObject* parent)
    : QObject(parent) {
    connect(&udpSocket, &QUdpSocket::readyRead, this, &UdpDiscoveryService::processPendingDatagrams);
}

void UdpDiscoveryService::startDiscovery() {
    udpSocket.bind(10001, QUdpSocket::ShareAddress);
    qDebug() << "Started UDP discovery on port 10001.";
}

void UdpDiscoveryService::processPendingDatagrams() {
    while (udpSocket.hasPendingDatagrams()) {
        QByteArray datagram;
        datagram.resize(int(udpSocket.pendingDatagramSize()));
        QHostAddress sender;
        quint16 senderPort;

        udpSocket.readDatagram(datagram.data(), datagram.size(), &sender, &senderPort);
        emit serverDiscovered(sender.toString() + ":" + QString::number(senderPort));
    }
}
