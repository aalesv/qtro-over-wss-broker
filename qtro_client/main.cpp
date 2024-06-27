#include <QCoreApplication>
#include <QRemoteObjectNode>
#include "QtWebSockets/QtWebSockets"
#include <iostream>
#include "websocketiodevice.h"
//#include "qtro.hpp"
#include "qtrowrapper.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    QString addr = "127.0.0.1:33315";
    if (argc == 2)
        addr = QString(argv[1]);

    std::cout << "Remote address is " << addr.toStdString() << std::endl;

    QScopedPointer<QtroWrapper> qtroWrapper{new QtroWrapper(addr)};

    return a.exec();
}
