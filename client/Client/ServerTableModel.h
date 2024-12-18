#ifndef SERVERTABLEMODEL_H
#define SERVERTABLEMODEL_H

#include <QAbstractTableModel>
#include <QVector>
#include <QTimer>
#include <QDateTime>
#include <QDebug>
#include <grpcpp/grpcpp.h>
#include "api.grpc.pb.h"
#include <QList>
#include <memory>
#include "GrpcClient.h"
#include <QTime>
#include <QColor>

struct ServerEntry {
    QString address;
    QString lastPingTime;
    QString status;
};

class ServerTableModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit ServerTableModel(QObject* parent = nullptr);

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole) override;
    Qt::ItemFlags flags(const QModelIndex& index) const override;

    void addServer(const QString& address);
    void updateLastPingTime(int row, const QString& time);
    void updateStatus(int row, const QString& status);
    //void updatePingTime(const QString& address, const QString& time);
    //void updateStatus(const QString& address, const QString& status);
    int findRowByAddress(const QString& address) const;

private:
    QList<ServerEntry> servers;
};

#endif // SERVERTABLEMODEL_H
