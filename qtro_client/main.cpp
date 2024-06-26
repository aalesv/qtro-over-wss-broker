#include <QCoreApplication>
#include <QRemoteObjectNode>
#include "QtWebSockets/QtWebSockets"
#include <iostream>
#include "websocketiodevice.h"
#include "qtro.hpp"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    QString addr = "127.0.0.1:33315";
    if (argc == 2)
        addr = QString(argv[1]);

    std::cout << "Broker address is " << addr.toStdString() << std::endl;


    QScopedPointer<QWebSocket> webSocket{new QWebSocket};

    QSslConfiguration sslConfiguration;
    sslConfiguration.setPeerVerifyMode(QSslSocket::VerifyNone);
    webSocket->setSslConfiguration(sslConfiguration);

    //Transport for remote object protocol
    WebSocketIoDevice socket(webSocket.data());

    QtroRemote qtro;

    //Node in host mode
    QRemoteObjectHost node;
    //Mandatory
    node.setHostUrl(QUrl("node_url"), QRemoteObjectHost::AllowExternalRegistration);
    node.setHeartbeatInterval(1000);
    //Enable remote object access
    node.enableRemoting(&qtro, QStringLiteral("Test1"));
    QObject::connect(webSocket.data(), &QWebSocket::textMessageReceived, &node,
                     [&](QString message)
                     {
                        //Run host node after connection is up
                        if (message == "FastECU_PTP_Autodiscovery")
                            node.addHostSideConnection(&socket);
                     }
    );
    webSocket->open("wss://" + addr);

    return a.exec();
}
