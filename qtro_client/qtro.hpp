#ifndef QTRO_HPP
#define QTRO_HPP
#include <QCoreApplication>
//This file is autogenerated from qtro.rep
#include "rep_qtro_source.h"

class SerialPortActions : public QObject
{
    Q_OBJECT
public:
    explicit SerialPortActions(){}
    ~SerialPortActions(){}
    int B = 0;
    long someFunc(QString s)
        {qDebug() << s; return 42;}
};

//Class QtroRemoteSimpleSource is autogenerated
class QtroRemote : public QtroRemoteSimpleSource
{
    Q_OBJECT
public:
    explicit QtroRemote(QObject *parent=nullptr)
        : QtroRemoteSimpleSource(parent)
        , serialPort(QSharedPointer<SerialPortActions>(new SerialPortActions))
        {qDebug() << "Server started";}
    ~QtroRemote()
        {qDebug() << "Server stopped";}
    //Class methods must be implemented
    //Getters and seeters for properties are autogenerated
    long someFunc(QString s) override
        {return serialPort->someFunc(s);}
    int get_B() override
        {return serialPort->B;}
    bool set_B(int val) override
        {serialPort->B=val;return true;}
private:
    QSharedPointer<SerialPortActions> serialPort;
};

class QtroRemoteUtility : public QtroRemoteUtilitySimpleSource
{
    Q_OBJECT
public:
    explicit QtroRemoteUtility(QObject *parent=nullptr)
        : QtroRemoteUtilitySimpleSource(parent)
        {qDebug() << "Utility started";}
    ~QtroRemoteUtility()
        {qDebug() << "Utility stopped";}
    bool send_log_window_message(QString message) override
        {qDebug() << "Received log message" << message; return true;}
};

#endif // QTRO_HPP
