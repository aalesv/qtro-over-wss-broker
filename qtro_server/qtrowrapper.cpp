// This is an open source non-commercial project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com

#include "qtrowrapper.h"

QtroWrapper::QtroWrapper(QString peerAddress, QObject *parent)
    : QObject{parent}
    , peerAddress(peerAddress)
    , webSocket(new QWebSocket("",QWebSocketProtocol::VersionLatest,this))
    //Different nodes could run over the same WebSocket
    , socket_remote(new WebSocketIoDevice(webSocket, webSocket))
    , socket_remote_utility(new WebSocketIoDevice(webSocket, webSocket))
{
    if (this->peerAddress.startsWith("local:"))
    {
        startLocal();
    }
    else
    {
        startOverNetwok();
    }
}

QtroWrapper::~QtroWrapper()
{}

void QtroWrapper::startOverNetwok(void)
{
    QSslConfiguration sslConfiguration;
    sslConfiguration.setPeerVerifyMode(QSslSocket::VerifyNone);
    webSocket->setSslConfiguration(sslConfiguration);
    QObject::connect(webSocket, &QWebSocket::connected, this,
                     [&]()
                     {
                         //Run client node after socket is up
                         node_remote.addClientSideConnection(socket_remote);
                         node_remote_utility.addClientSideConnection(socket_remote_utility);
                         sendAutoDiscoveryMessage();
                     });
    node_remote.setHeartbeatInterval(1000);
    node_remote_utility.setHeartbeatInterval(1000);
    QObject::connect(webSocket, QOverload<QAbstractSocket::SocketError>::of(&QWebSocket::error),
                     [=](QAbstractSocket::SocketError error){ qDebug() << error; });
    //WebSocket over SSL
    webSocket->open("wss://" + peerAddress);

    //Connect to source published with name "Test1"
    //Class QtroRemoteReplica is autogenerated from qtro.rep
    qtro_remote.reset(
        node_remote.acquire<QtroRemoteReplica>(remoteObjectName)
    );
    qtro_remote_utility.reset(
        node_remote_utility.acquire<QtroRemoteUtilityReplica>(remoteObjectNameUtility)
    );
    //Wait for replication
    while (!qtro_remote->waitForSource(10000))
    {
        qDebug() << "Sending autodiscovery message";
        sendAutoDiscoveryMessage();
    }
    qDebug() << "QtroRemoteReplica connected";
    if (qtro_remote_utility->waitForSource(10000))
    {
        qDebug() << "QtroRemoteUtilityReplica connected";
    }
    else
        qDebug() << "QtroRemoteUtilityReplica NOT connected";
}

void QtroWrapper::startLocal(void)
{
    //Each node should run over distinct named pipe
    QString remote_url = peerAddress+remoteObjectName;
    QString utility_url = peerAddress+remoteObjectNameUtility;

    node_remote.connectToNode(QUrl(remote_url));
    qtro_remote.reset(
        node_remote.acquire<QtroRemoteReplica>(remoteObjectName)
    );

    node_remote_utility.connectToNode(QUrl(utility_url));
    qtro_remote_utility.reset(
        node_remote_utility.acquire<QtroRemoteUtilityReplica>(remoteObjectNameUtility)
        );

    while (!qtro_remote->waitForSource(10000))
    {}
    qDebug() << "QtroRemoteReplica connected";
    if (qtro_remote_utility->waitForSource(10000))
    {
        qDebug() << "QtroRemoteUtilityReplica connected";
    }
    else
        qDebug() << "QtroRemoteUtilityReplica NOT connected";
}

void QtroWrapper::sendAutoDiscoveryMessage()
{
    if (webSocket->isValid())
    {
        webSocket->sendTextMessage(autodiscoveryMessage);
    }

}
