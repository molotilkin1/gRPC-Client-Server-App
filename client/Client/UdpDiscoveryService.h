#ifndef UDPDISCOVERYSERVICE_H
#define UDPDISCOVERYSERVICE_H

#include <QObject>
#include <QUdpSocket>

class UdpDiscoveryService : public QObject
{
    Q_OBJECT

public:
    explicit UdpDiscoveryService(QObject* parent = nullptr);
    void startDiscovery();

signals:
    void serverDiscovered(const QString& address);
    void discoveryStopped();

private slots:
    void processPendingDatagrams();

private:
    QUdpSocket udpSocket;
};

#endif // UDPDISCOVERYSERVICE_H
