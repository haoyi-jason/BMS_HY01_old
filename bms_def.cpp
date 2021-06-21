#include <QObject>
#include<QJsonDocument>
#include<QJsonObject>
#include<QJsonValue>
#include<QJsonArray>
#include "bms_model.h"
#include "bms_system.h"



void RemoteSystem::setDigitalOut(int id, int value){
    QString msg = QString("BCU:DO:%1:%2").arg(id).arg(value);
    writeCommand(msg);
}

void RemoteSystem::setVoltageSource(int id, int value){
    QString msg = QString("BCU:VO:%1:%2").arg(id).arg(value);
    writeCommand(msg);
}

void RemoteSystem::openSerialPort(QString name, int baudrate, int parity, int stopBits){
    QString msg = QString("PORT:OPEN:%1:%2:%3:%4").arg(name).arg(baudrate).arg(parity).arg(stopBits);
    writeCommand(msg);
}

void RemoteSystem::closeSerialPort(QString name){
    QString msg = QString("PORT:CLOSE:%1").arg(name);
    writeCommand(msg);
}

void RemoteSystem::writeSerialPort(QString name, QString data){
    QString msg = QString("PORT:WRITE:%1:%2").arg(name).arg(data);
    writeCommand(msg);

}
void RemoteSystem::writeCommand(QString command){
    if(socket != nullptr){
        socket->write(command.toUtf8());
    }
}

void RemoteSystem::logData(QByteArray b){
    QString path = this->logPath + "/" + QDateTime::currentDateTime().toString("yyyyMMdd_hhmmss.bin");
    QFile f(path);
    if(f.open(QIODevice::ReadWrite)){
        //f.write(d);
        QDataStream df(&f);
        df << b;
        f.close();
    }

    // remove old files, keep 1000 only
    QDir dir(this->logPath);
    if(dir.count() > 110){
        QFileInfoList files = dir.entryInfoList(QDir::Files | QDir::NoDotAndDotDot, QDir::Time | QDir::Reversed);
        for(int i=files.size();i>100;--i){
            const QFileInfo& info = files.at(i-1);
            QFile::remove(info.absoluteFilePath());
        }
    }

}
