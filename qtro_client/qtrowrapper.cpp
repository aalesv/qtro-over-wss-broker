#include "qtrowrapper.h"

QtroWrapper::QtroWrapper(QString peerAddress, QObject *parent)
    : QObject{parent}
    , peerAddress(peerAddress)
    , webSocket(new QWebSocket("",QWebSocketProtocol::VersionLatest,this))
    //Different nodes could run over the same WebSocket
    , socket_remote(new WebSocketIoDevice(webSocket, webSocket))
    , socket_utility(new WebSocketIoDevice(webSocket, webSocket))
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
    //Mandatory
    node_remote.setHostUrl(QUrl(nodeUrl), QRemoteObjectHost::AllowExternalRegistration);
    node_utility.setHostUrl(QUrl(nodeUrlUtility), QRemoteObjectHost::AllowExternalRegistration);
    node_remote.setHeartbeatInterval(heartbeatInterval);
    node_utility.setHeartbeatInterval(heartbeatInterval);
    //Enable remote object access
    node_remote.enableRemoting(&qtro_remote, remoteObjectName);
    node_utility.enableRemoting(&qtro_utility, remoteObjectNameUtility);
    QObject::connect(webSocket, &QWebSocket::textMessageReceived, this,
                     [&](QString message)
                     {
                         //Run host node after connection is up
                         if (message == autodiscoveryMessage)
                         {
                             node_remote.addHostSideConnection(socket_remote);
                             node_utility.addHostSideConnection(socket_utility);
                         }
                     }
                     );
    webSocket->open("wss://" + peerAddress);
}

void QtroWrapper::startLocal(void)
{
    //Each node should run over distinct named pipe
    QString remote_url = peerAddress+remoteObjectName;
    QString utility_url = peerAddress+remoteObjectNameUtility;

    node_remote.setHostUrl(QUrl(remote_url),
                    QRemoteObjectHost::BuiltInSchemasOnly);
    node_remote.enableRemoting(&qtro_remote, remoteObjectName);

    node_utility.setHostUrl(QUrl(utility_url),
                    QRemoteObjectHost::BuiltInSchemasOnly);
    node_utility.enableRemoting(&qtro_utility, remoteObjectNameUtility);
}
