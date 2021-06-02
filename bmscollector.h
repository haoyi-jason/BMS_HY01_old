#ifndef BMSCOLLECTOR_H
#define BMSCOLLECTOR_H

#include <QObject>

class QTcpSocket;
class BMS_SystemInfo;
class BMS_System;
class RemoteSystem;
//{
//public:
//    QString connection="";
//    QTcpSocket *socket = nullptr;
//    BMS_SystemInfo *system = nullptr;
//    bool configReady = false; // should be false
//    QByteArray data;
//};

class BMSCollector : public QObject
{
    Q_OBJECT
public:
    explicit BMSCollector(QObject *parent = nullptr);

    bool addConnection(QString ipAddress);
    bool deleteConnection(QTcpSocket *socket);
    bool readConfig(QString connection);
    void readAllConfig();
    RemoteSystem *currentSystem();
    RemoteSystem *nextSystem();
    RemoteSystem *prevSystem();
    bool loadConfig(QString path); // load from local system
    bool connectServer(int id);
    bool disconnectServer(int id);
signals:
    void configReady();
    void dataReceived();

public slots:
    void handleServerData();

private:
    QList<RemoteSystem*> m_servers;
    int m_currentSystemIndex = -1;
};

#endif // BMSCOLLECTOR_H
