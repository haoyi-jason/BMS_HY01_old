#include "bmscollector.h"
#include "bms_def.h"
#include <QtNetwork>
#include "secs.h"
#include "bms_bmudevice.h"
#include "bms_bcudevice.h"
#include "bms_svidevice.h"
#include "bms_system.h"
#include <QSysInfo>

BMSCollector::BMSCollector(QObject *parent) : QObject(parent)
{

}

bool BMSCollector::loadConfig(QString path)
{
    bool ret = false;

    QFile f(path);
    if(!f.open(QIODevice::ReadOnly)){
        f.close();
        return false;
    }
    QJsonParseError e;
    QJsonDocument d = QJsonDocument::fromJson(f.readAll(),&e);
    f.close();
    if(d.isNull()){
        return false;
    }
    QJsonObject obj = d.object();

    if(obj.contains("bms")){
        QJsonArray lst = obj["bms"].toArray();
        for(auto v:lst){
            QJsonObject o = v.toObject();
            RemoteSystem *sys =  new RemoteSystem();
            sys->connection = o["ip"].toString();
            sys->alias = o["alias"].toString();
            sys->port = o["port"].toInt();
            if(QSysInfo::productType().contains("win")){
                sys->logPath = o["log_path"].toString()+"/"+sys->connection;
            }
            else{
                sys->logPath = QCoreApplication::applicationDirPath()+ o["log_path"].toString()+"/"+sys->connection;
            }
            sys->system = new BMS_System();
            sys->autoConnect = o["autoconnect"].toBool();
            m_servers.append(sys);

            // create log path
            if(!QDir(sys->logPath).exists()){
                QDir().mkpath(sys->logPath);
            }
        }
    }
}

bool BMSCollector::connectServer(int id)
{
    if(id == -1){ // connect each system if autoconnect
        foreach (RemoteSystem *s, m_servers) {
            if(!s->autoConnect) continue;
            if(s->socket == nullptr){
                QTcpSocket *socket = new QTcpSocket();
                socket->connectToHost(s->connection,s->port);
                if(socket->isValid()){
                    connect(socket,&QTcpSocket::readyRead,this,&BMSCollector::handleServerData);
                    if(m_currentSystemIndex<0) m_currentSystemIndex = 0;
                    s->socket = socket;
                    readAllConfig();
                }
            }
        }
    }

    else if(id < m_servers.size()){
        RemoteSystem *s = m_servers[id];
        if(s->socket == nullptr){
            QTcpSocket *socket = new QTcpSocket();
            qDebug()<<"Connect to "<<s->connection <<" Port: " <<s->port;
            socket->connectToHost(s->connection,s->port);
            bool connected = socket->waitForConnected(5000);
            if(connected){
                qDebug()<<"Socket connected";
                connect(socket,&QTcpSocket::readyRead,this,&BMSCollector::handleServerData);
                if(m_currentSystemIndex<0) m_currentSystemIndex = 0;
                s->socket = socket;
                readAllConfig();
            }
            else{
                qDebug()<<"Connect fail";
            }
        }
    }
    return true;
}

bool BMSCollector::disconnectServer(int id)
{
    qDebug()<<"Disconnect:"<<id;
    if(id < m_servers.size() && m_servers[id]->socket != nullptr){
        if(m_servers[id]->socket->isValid()){
            m_servers[id]->socket->close();
            m_servers[id]->socket->deleteLater();
            m_servers[id]->socket = nullptr;
            m_servers[id]->configReady = false;
            return true;
        }
    }
    return false;
}

bool BMSCollector::addConnection(QString ipAddress)
{
//    QTcpSocket *socket = new QTcpSocket();
//    socket->connectToHost(ipAddress,5001);
//    if(socket->isValid()){
//        RemoteSystem *sys = new RemoteSystem();
//        sys->connection=ipAddress;
//        sys->socket = socket;
//        sys->system = new BMS_System();
//        sys->system->connectionString = ipAddress;
//        m_servers.append(sys);
//        connect(socket,&QTcpSocket::readyRead,this,&BMSCollector::handleServerData);
//        if(m_currentSystemIndex<0) m_currentSystemIndex = 0;
//        return true;
//    }
    return false;
}

bool BMSCollector::deleteConnection(QTcpSocket *socket)
{
    foreach (RemoteSystem *s, m_servers) {
        if(s->socket == socket){
            m_servers.removeOne(s);
        }
    }
}

bool BMSCollector::readConfig(QString connection)
{
    QByteArray b = "READ:CFG";
    //b.insert(0,hsmsParser::genHeader(hsmsParser::BMS_CONFIG,b.size()));
    foreach(RemoteSystem *s, m_servers) {
        if(s->connection == connection){
            qDebug()<<"Write Command"<<b;
            s->socket->write(b);
//            s->socket->flush();
        }
    }
}

void BMSCollector::readAllConfig()
{
    QByteArray b = "READ:CFG";
    //b.insert(0,hsmsParser::genHeader(hsmsParser::BMS_CONFIG,b.size()));
    foreach(RemoteSystem *s, m_servers) {
        qDebug()<<"Write Command"<<b;
//        s->socket->flush();
        s->socket->write(b);
    }
}

// todo: move data handling process to timed function
// handle every system periodically
void BMSCollector::handleServerData()
{
    QTcpSocket *socket = (QTcpSocket*)sender();
    //RemoteSystem *sys;
    foreach (RemoteSystem *sys, m_servers) {
        if(sys->socket == socket){
            //QByteArray data = sys->socket->readAll();
            sys->data.append(sys->socket->readAll());

            int len;
            quint8 hlen;
            if(sys->configReady){
                qDebug()<<"Config ready";
                quint8 ret = hsmsParser::getHeader(sys->data,&len,&hlen);
                if(ret == hsmsParser::BMS_STACK){
                    //if(sys->data.size() >= len){
                        qDebug()<<"Receive stack packet";
                        sys->data.remove(0,hlen);
                        QDataStream d(&sys->data,QIODevice::ReadOnly);
                        d >> sys->system;
                        emit dataReceived();
                        sys->data.remove(0,len);
                        QByteArray b;
                        QDataStream d2(&b,QIODevice::ReadWrite);
                        d2 << sys->system;
                        sys->logData(b);
                    //}
                }
                else if(ret == hsmsParser::BMS_WRONG_HEADER){
                    sys->data.clear();
                }
            }
            else{
                qDebug()<<"Not Ready";
                quint8 ret = hsmsParser::getHeader(sys->data,&len,&hlen);
                if(ret == hsmsParser::BMS_CONFIG){

                    qDebug()<<"Receive config packet";
                    sys->data.remove(0,hlen);
                    sys->system->Configuration(sys->data);
                    sys->configReady = true;
                    emit configReady();
                    sys->data.remove(0,len);
                }
                else if(ret == hsmsParser::BMS_WRONG_HEADER){
                    sys->data.clear();
                }
            }
        }

    }
}

RemoteSystem* BMSCollector::currentSystem()
{
    if(m_currentSystemIndex < 0){
        return nullptr;
    }
    else{
        return m_servers.at(m_currentSystemIndex);
    }
}

RemoteSystem* BMSCollector::nextSystem()
{
    if(m_currentSystemIndex < 0) return nullptr;
    else{
        m_currentSystemIndex++;
        if(m_currentSystemIndex == m_servers.size()){
            m_currentSystemIndex = 0;
        }
        return m_servers.at(m_currentSystemIndex);
    }

}

RemoteSystem* BMSCollector::prevSystem()
{
    if(m_currentSystemIndex < 0) return nullptr;
    else{
        m_currentSystemIndex--;
        if(m_currentSystemIndex<0){
            m_currentSystemIndex = m_servers.size()-1;
        }
        return m_servers.at(m_currentSystemIndex);
    }
}

