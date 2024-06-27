#ifndef QTROWRAPPER_H
#define QTROWRAPPER_H

#include <QObject>
#include <QSharedPointer>
#include <QWebSocket>
#include "websocketiodevice.h"
#include "qtro.hpp"

class QtroWrapper : public QObject
{
    Q_OBJECT
public:
    explicit QtroWrapper(QString peerAddress, QObject *parent = nullptr);
    ~QtroWrapper();
    const QString autodiscoveryMessage = "FastECU_PTP_Autodiscovery";
    const QString nodeUrl = "node_url";
    const QString remoteObjectName = "Test1";
    const int heartbeatInterval = 1000;

signals:
private:
    QString peerAddress;
    QSharedPointer<QWebSocket> webSocket{new QWebSocket};
    WebSocketIoDevice socket{webSocket.data()};
    QtroRemote qtro;
    QRemoteObjectHost node;
    void startOverNetwok(void);
    void startLocal(void);
};

#endif // QTROWRAPPER_H
