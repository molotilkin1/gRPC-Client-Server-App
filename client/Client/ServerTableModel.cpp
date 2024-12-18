#include "ServerTableModel.h"

ServerTableModel::ServerTableModel(QObject* parent)
    : QAbstractTableModel(parent) {}

int ServerTableModel::rowCount(const QModelIndex& parent) const {
    Q_UNUSED(parent);
    return servers.count();
}

int ServerTableModel::columnCount(const QModelIndex& parent) const {
    Q_UNUSED(parent);
    return 4;
}

QVariant ServerTableModel::data(const QModelIndex& index, int role) const {
    /*if (!index.isValid() || role != Qt::DisplayRole) return QVariant();

    const auto& server = servers[index.row()];
    switch (index.column()) {
        case 0: return server.address;
        case 1: return server.lastPingTime;
        case 2: return server.status;
        case 3: return server.status == "Online" ? "Disconnect" : "Connect";
    }
    return QVariant();*/
    if (!index.isValid()) return {}/*QVariant()*/;

    const auto& server = servers[index.row()];
    // Изменение цвета текста для статуса
    if (role == Qt::ForegroundRole && index.column() == 2) {
        return server.status == "Online" ? QColor(Qt::green) : QColor(Qt::red);
    }
    if (role == Qt::DisplayRole) {
        switch (index.column()) {
        case 0: return server.address; break;
        case 1: return server.lastPingTime.isEmpty() ? "-" : server.lastPingTime;break;
        case 2: return server.status;break;
        case 3: return server.status == "Online" ? "Disconnect" : "Connect";break;//"Online" ? "Disconnect" : "Connect"
        }
    }



    return {};
}

QVariant ServerTableModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        switch (section) {
            case 0: return "IP";
            case 1: return "Last ping time";
            case 2: return "Status";
            case 3: return "Action";
        }
    }
    return {};//return QVariant();
}

bool ServerTableModel::setData(const QModelIndex& index, const QVariant& value, int role) {
    if (!index.isValid() || index.row() >= servers.size() || role != Qt::EditRole)
        return false;

    auto& server = servers[index.row()];

    switch (index.column()) {
    case 1:
        server.lastPingTime = value.toString();
        break;
    case 2:
        server.status = value.toString();
        break;
    default:
        return false;
    }

    emit dataChanged(index, index);
    return true;
}

Qt::ItemFlags ServerTableModel::flags(const QModelIndex& index) const {
    if (!index.isValid())
        return Qt::NoItemFlags;

    return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
}

void ServerTableModel::addServer(const QString& address) {
    // Проверяем, существует ли уже этот сервер
    if (findRowByAddress(address) != -1) {
        return; // Сервер уже добавлен
    }
    beginInsertRows(QModelIndex(), servers.size(), servers.size());
    servers.append({ address, QDateTime::currentDateTime().toString("hh:mm:ss"), "Offline" });
    endInsertRows();
}


void ServerTableModel::updateLastPingTime(int row, const QString& time) {
    if (row >= 0 && row < servers.size()) {
        servers[row].lastPingTime = time;
        emit dataChanged(index(row, 1), index(row, 1)); // Update "Last ping time" column
    }
}

void ServerTableModel::updateStatus(int row, const QString& status) {
    if (row >= 0 && row < servers.size()) {
        servers[row].status = status;
        emit dataChanged(index(row, 2), index(row, 3), {Qt::DisplayRole, Qt::ForegroundRole}); // Update "Status" column
    }
}

/*void ServerTableModel::updatePingTime(const QString& address, const QString& time) {
    for (int i = 0; i < servers.size(); ++i) {
        if (servers[i].address == address) {
            servers[i].lastPingTime = time;
            QModelIndex index = createIndex(i, 1);
            emit dataChanged(index, index);
            break;
        }
    }
}

void ServerTableModel::updateStatus(const QString& address, const QString& status) {
    for (int i = 0; i < servers.size(); ++i) {
        if (servers[i].address == address) {
            servers[i].status = status;
            QModelIndex index = createIndex(i, 2);
            emit dataChanged(index, index);
            break;
        }
    }
}*/

int ServerTableModel::findRowByAddress(const QString& address) const {
    for (int i = 0; i < servers.size(); ++i) {
        if (servers[i].address == address) {
            return i;
        }
    }
    return -1;
}
