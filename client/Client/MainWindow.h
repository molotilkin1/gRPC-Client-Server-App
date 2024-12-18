#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTableView>
#include <QPushButton>
#include <QVBoxLayout>
#include "ServerTableModel.h"
#include "UdpDiscoveryService.h"
#include "GrpcClient.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);

private slots:
    void onStartDiscovery();
    void onServerDiscovered(const QString& address);
    void onActionButtonClicked(int row);

private:
    QWidget* centralWidget;
    QTableView* tableView;
    QPushButton* startDiscoveryButton;
    QVBoxLayout* layout;

    UdpDiscoveryService udpService;
    ServerTableModel serverTableModel;
};
#endif // MAINWINDOW_H
