QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    GrpcClient.cpp \
    ServerTableModel.cpp \
    UdpDiscoveryService.cpp \
    api.grpc.pb.cc \
    api.pb.cc \
    main.cpp \
    MainWindow.cpp

HEADERS += \
    GrpcClient.h \
    MainWindow.h \
    ServerTableModel.h \
    UdpDiscoveryService.h \
    api.grpc.pb.h \
    api.pb.h

FORMS += \
    MainWindow.ui

# Линковка с gRPC и Protobuf
LIBS += -lgrpc++ -lgrpc -lprotobuf
# Указать путь к gRPC и Protobuf заголовкам
INCLUDEPATH += /usr/include
INCLUDEPATH += /usr/local/include

# Указать путь к gRPC и Protobuf библиотекам
LIBS += -L/usr/lib
LIBS += -L/usr/local/lib


# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
