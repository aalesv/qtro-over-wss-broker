#include "qtrowrapper.h"

QtroWrapper::QtroWrapper(QString peerAddress, QObject *parent)
    : QObject{parent}
    , peerAddress(peerAddress)
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
    QObject::connect(webSocket.data(), &QWebSocket::connected, &node,
                     [&]()
                     {
                         //Run client node after socket is up
                         node.addClientSideConnection(&socket);
                         sendAutoDiscoveryMessage();
                     });
    node.setHeartbeatInterval(1000);
    QObject::connect(webSocket.data(), QOverload<QAbstractSocket::SocketError>::of(&QWebSocket::error),
                     [=](QAbstractSocket::SocketError error){ qDebug() << error; });
    //WebSocket over SSL
    webSocket->open("wss://" + peerAddress);

    //Connect to source published with name "Test1"
    //Class QtroRemoteReplica is autogenerated from qtro.rep
    qtro_remote.reset(
        node.acquire<QtroRemoteReplica>(remoteObjectName)
    );
    //Wait for replication
    while (!qtro_remote->waitForSource(10000))
    {
        sendAutoDiscoveryMessage();
    }

}

void QtroWrapper::startLocal(void)
{
    node.connectToNode(QUrl(peerAddress));
    qtro_remote.reset(
        node.acquire<QtroRemoteReplica>(remoteObjectName)
    );
    while (!qtro_remote->waitForSource(10000))
    {}
}

void QtroWrapper::sendAutoDiscoveryMessage()
{
    if (webSocket->isValid())
    {
        webSocket->sendTextMessage(autodiscoveryMessage);
    }

}
