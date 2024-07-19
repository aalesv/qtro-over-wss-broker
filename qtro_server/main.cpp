// This is an open source non-commercial project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com

#include <QCoreApplication>
#include <QRemoteObjectNode>
#include "QtWebSockets/QtWebSockets"
#include <iostream>
#include "websocketiodevice.h"
#include "qtrowrapper.h"

void sendAutoDiscoveryMessage(QWebSocket *webSocket)
{
    if (webSocket->isValid())
    {
        webSocket->sendTextMessage("FastECU_PTP_Autodiscovery");
    }

}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    QString addr = "127.0.0.1:33314";
    if (argc == 2)
        addr = QString(argv[1]);

    std::cout << "Remote address is " << addr.toStdString() << std::endl;

    QtroWrapper qtro_remote(addr);

    qDebug() << "someFunc";
    long r = qtro_remote.someFunc("qqq");
    qDebug() << r;

    qtro_remote.send_log_window_message("Remote utility connection works well");

    qDebug() << qtro_remote.someFunc("hello");
    qDebug() << "set_B";
    qtro_remote.set_B(321);
    qDebug() << qtro_remote.get_B();

    return a.exec();
}
