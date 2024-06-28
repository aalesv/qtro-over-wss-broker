#include "qtrowrapper.h"

QtroWrapper::QtroWrapper(QString peerAddress, QObject *parent)
    : QObject{parent}
    , peerAddress(peerAddress)
    , webSocket(new QWebSocket("",QWebSocketProtocol::VersionLatest,this))
    , socket(new WebSocketIoDevice(webSocket, webSocket))
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
    node.setHostUrl(QUrl(nodeUrl), QRemoteObjectHost::AllowExternalRegistration);
    node.setHeartbeatInterval(heartbeatInterval);
    //Enable remote object access
    node.enableRemoting(&qtro, remoteObjectName);
    QObject::connect(webSocket, &QWebSocket::textMessageReceived, &node,
                     [&](QString message)
                     {
                         //Run host node after connection is up
                         if (message == autodiscoveryMessage)
                             node.addHostSideConnection(socket);
                     }
                     );
    webSocket->open("wss://" + peerAddress);
}

void QtroWrapper::startLocal(void)
{
    node.setHostUrl(QUrl(peerAddress),
                    QRemoteObjectHost::BuiltInSchemasOnly);
    node.enableRemoting(&qtro, remoteObjectName);
}
