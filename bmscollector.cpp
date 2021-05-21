#include "bmscollector.h"
#include "bms_def.h"
#include <QtNetwork>
#include "secs.h"

BMSCollector::BMSCollector(QObject *parent) : QObject(parent)
{

}

bool BMSCollector::addConnection(QString ipAddress)
{
    QTcpSocket *socket = new QTcpSocket();
    socket->connectToHost(ipAddress,5001);
    if(socket->isValid()){
        RemoteSystem *sys = new RemoteSystem();
        sys->connection=ipAddress;
        sys->socket = socket;
        sys->system = new BMS_SystemInfo();
        sys->system->connectionString = ipAddress;
        m_servers.append(sys);
        connect(socket,&QTcpSocket::readyRead,this,&BMSCollector::handleServerData);
        if(m_currentSystemIndex<0) m_currentSystemIndex = 0;
        return true;
    }
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
            s->socket->write(b);
        }
    }
}

void BMSCollector::handleServerData()
{
    QTcpSocket *socket = (QTcpSocket*)sender();
    //RemoteSystem *sys;
    foreach (RemoteSystem *sys, m_servers) {
        if(sys->socket == socket){
            QByteArray data = sys->socket->readAll();
            int len;
            quint8 hlen;
            if(sys->configReady){
                qDebug()<<"Config ready";
                if(hsmsParser::getHeader(data,&len,&hlen) == hsmsParser::BMS_STACK){
                    qDebug()<<"Receive stack packet";
                    data.remove(0,hlen);
                    QDataStream d(&data,QIODevice::ReadOnly);
                    d >> sys->system;
                    emit dataReceived();
                }
            }
            else{
                qDebug()<<"Not Ready";
                if(hsmsParser::getHeader(data,&len,&hlen) == hsmsParser::BMS_CONFIG){

                    qDebug()<<"Receive config packet";
                    sys->system->Configuration(data.mid(hlen));
                    sys->configReady = true;
                    emit configReady();
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

