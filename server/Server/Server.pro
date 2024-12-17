QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

LIBS += -lprotobuf -lgrpc++ -lgrpc

SOURCES += \
    main.cpp \
    ServerWindow.cpp \
    api.grpc.pb.cc \
    api.pb.cc

HEADERS += \
    ServerWindow.h \
    api.grpc.pb.h \
    api.pb.h

FORMS += \
    ServerWindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
