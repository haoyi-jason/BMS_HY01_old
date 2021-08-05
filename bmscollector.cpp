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

            sys->autoConnect = o["autoconnect"].toBool();
            m_servers.append(sys);

            if(QSysInfo::productType().contains("win")){
                sys->logPath = o["log_path"].toString()+"/"+sys->connection;
            }
            else{
                sys->logPath = QCoreApplication::applicationDirPath()+ o["log_path"].toString()+"/"+sys->connection;
            }
            //sys->system->logPath(sys->logPath);

            // create log path
            if(!QDir(sys->logPath).exists()){
                QDir().mkpath(sys->logPath);
            }

            if(o.contains("enable_log")){
                //sys->system->enableLog(o["enable_log"].toBool());
                sys->enableLog = o["enable_log"].toBool();
            }
            else{
                //sys->system->enableLog(false);

            }

            if(o.contains("log_days")){
                //sys->system->logDays(o["log_days"].toInt());
                sys->logDays = o["log_days"].toInt();
            }
            else{
                //sys->system->logDays(-1);
            }

            if(o.contains("log_records")){
                //sys->system->logRecords(o["log_records"].toInt());
                sys->logRecords = o["log_records"].toInt();
            }
            else{
                //sys->system->logRecords(1000);
            }
        }
    }

    if(obj.contains("loginpromote")){
        m_loginPromote = obj["loginpromote"].toBool();
    }
}

bool BMSCollector::loginPromote()
{
    return m_loginPromote;
}

bool BMSCollector::connectServer(int id)
{
    bool success = false;
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
                    s->data.clear();
                    s->configReady = false;
                    //s->configReady  =true;
                    readAllConfig();
                    success = true;
                }
            }
        }
    }

    else if(id < m_servers.size()){
        RemoteSystem *s = m_servers[id];
        if(s->socket == nullptr){
            QTcpSocket *socket = new QTcpSocket();
            //qDebug()<<"Connect to "<<s->connection <<" Port: " <<s->port;
            socket->connectToHost(s->connection,s->port);
            bool connected = socket->waitForConnected(5000);
            if(connected){
                qDebug()<<"Socket connected";
                connect(socket,&QTcpSocket::readyRead,this,&BMSCollector::handleServerData);
                if(m_currentSystemIndex<0) m_currentSystemIndex = 0;
                s->socket = socket;
                connect(socket,&QTcpSocket::disconnected,this,&BMSCollector::handleSocketDisconnect);
                s->configReady = false;
                s->data.clear();
                s->system = new BMS_System();
                s->system->setController(false);
                //s->configReady = true;
                readAllConfig();
                success = true;
            }
            else{
                //qDebug()<<"Connect fail";
            }
        }
    }
    return success;
}

bool BMSCollector::disconnectServer(int id)
{
    qDebug()<<Q_FUNC_INFO;
    if(id < m_servers.size() && m_servers[id]->socket != nullptr){
        if(m_servers[id]->socket->isValid()){
            m_servers[id]->socket->close();
            //disconnect(m_servers[id]->socket,&QTcpSocket::readyRead);
//            m_servers[id]->socket->deleteLater();
//            m_servers[id]->socket = nullptr;
            m_servers[id]->configReady = false;
            //m_servers[id]->system->deleteLater();
            //delete m_servers[id]->socket;
            //delete m_servers[id]->system;
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
    QByteArray b = "SYS:CFGFR";
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
    QByteArray b = "SYS:CFGFR";
    //b.insert(0,hsmsParser::genHeader(hsmsParser::BMS_CONFIG,b.size()));
    foreach(RemoteSystem *s, m_servers) {
        //qDebug()<<"Write Command"<<b;
        s->socket->write(b);
    }
}

void BMSCollector::readInitTime()
{
    QByteArray b = "SYS:INIT_TIME";
    foreach (RemoteSystem *s, m_servers) {
        s->socket->write(b);
    }
}

void BMSCollector::handleSocketDisconnect()
{
    //qDebug()<<Q_FUNC_INFO;
    QTcpSocket *sock = static_cast<QTcpSocket*>(sender());
    foreach (RemoteSystem *sys , m_servers) {
        if(sys->socket == sock){
            sys->socket->deleteLater();
            sys->socket = nullptr;
            sys->configReady = false;

//            delete sys->system;
//            delete sys->socket;

            emit controllerOffline();
        }
    }
}

// todo: move data handling process to timed function
// handle every system periodically
void BMSCollector::handleServerData()
{
    QTcpSocket *socket = (QTcpSocket*)sender();
    //qDebug()<<Q_FUNC_INFO<<socket;
    //RemoteSystem *sys;
    foreach (RemoteSystem *sys, m_servers) {
        if(sys->socket == socket){
            //QByteArray data = sys->socket->readAll();
            sys->data.append(sys->socket->readAll());
            sys->lastSeen = QDateTime::currentDateTime();
            int len;
            quint8 hlen;
            if(sys->configReady){
                //qDebug()<<"Config ready";
                quint8 ret = hsmsParser::getHeader(sys->data,&len,&hlen);
                if(ret == hsmsParser::BMS_STACK && sys->data.size() >= (len+hlen)){
                    //if(sys->data.size() >= len){
                        //qDebug()<<"Receive stack packet";
                        sys->data.remove(0,hlen);
                        QDataStream d(&sys->data,QIODevice::ReadOnly);
                        //qDebug()<<"Feed start";
                        d >> sys->system;

                        //qDebug()<<"Feed 1";
                        emit dataReceived();
                        sys->data.remove(0,len);

                        //qDebug()<<"Feed 2";
//                        if(sys->system->enableLog()){
//                            QByteArray b;
//                            QDataStream d2(&b,QIODevice::ReadWrite);
//                            d2 << sys->system;
//                            sys->system->log(b);
//                        }
                        // show alarm information
                        //bool set = false;

                        //qDebug()<<"Feed end";
                    //}
                }
                else if(ret == hsmsParser::BMS_WRONG_HEADER){
                    sys->data.clear();
                }
                else if(ret == hsmsParser::BMS_SYS_DATETIME){
                    sys->data.remove(0,hlen);
                    QDataStream ds(&sys->data,QIODevice::ReadWrite);
                    qint64 epoch;
                    ds >> epoch;
                    sys->system->startTime(epoch);
                }
            }
            else{
                qDebug()<<"Not Ready";
                quint8 ret = hsmsParser::getHeader(sys->data,&len,&hlen);
                if(ret == hsmsParser::BMS_CONFIG){

                    qDebug()<<"Receive config packet";
                    sys->data.remove(0,hlen);
                    sys->system->Configuration2(sys->data);
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

