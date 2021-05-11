#include <QColor>
#include<QJsonDocument>
#include<QJsonObject>
#include<QJsonValue>
#include<QJsonArray>
#include "bms_model.h"

/***** BMS_BatteryInfo ************/
BMS_BatteryInfo::BMS_BatteryInfo(QObject *parent):
    QObject(parent)
{
    for(int i=0;i<bms::MAX_CELLS;i++){
        m_cellVoltage.append(0);
    }
    for(int i=0;i<bms::MAX_NTC;i++){
        m_packTemperature.append(0);
    }
}
BMS_BatteryInfo::BMS_BatteryInfo(int nofCells, int nofTemp, QObject *parent):
    QObject(parent)
{
    for(int i=0;i<nofCells;i++){
        m_cellVoltage.append(0);
        m_balancing.append(0);
    }
    for(int i=0;i<nofTemp;i++){
        m_packTemperature.append(0);
    }
}

ushort BMS_BatteryInfo::getCellVoltage(int index){
    if(index < m_cellVoltage.size()){
        return m_cellVoltage.at(index);
    }
    return 0;
}

ushort BMS_BatteryInfo::getPackTemperature(int index)
{
    if(index < m_packTemperature.size()){
        return m_packTemperature.at((index));
    }
    return 0;
}

/***** BMS_StackInfo **************/
BMS_StackInfo::BMS_StackInfo(){
}


int BMS_StackInfo::BatteryCount()
{
    return m_batteries.size();
}

ushort BMS_StackInfo::cellVoltage(int bid, int cid)
{
    if(bid < m_batteries.size()){
        BMS_BatteryInfo *info = m_batteries.at(bid);
        if(cid < 12){
            return info->getCellVoltage(cid);
        }
    }
    return 0;
}

void BMS_StackInfo::cellVoltage(int bid, int cid, ushort x)
{
    if(bid < m_batteries.size()){
        BMS_BatteryInfo *info = m_batteries.at(bid);
        if(cid < 12){
            info->cellVoltage(cid,x);
        }
    }
}

ushort BMS_StackInfo::queueData(int bid, int cid)
{
    if(bid < m_batteries.size()){
        BMS_BatteryInfo *info = m_batteries.at(bid);
        if(cid < 12){
            return info->getCellVoltage(cid);
        }
        else if(cid < 17){
            return info->getPackTemperature(cid-12);
        }
        else if(cid < 29){
            return info->balancing().at(cid-17);
        }
    }
    return 0;
}
void BMS_StackInfo::queueData(int bid, int cid, ushort x)
{
    if(bid < m_batteries.size()){
        BMS_BatteryInfo *info = m_batteries.at(bid);
        if(cid < 12){
            info->cellVoltage(cid,x);
        }
        else if(cid < 17){
            info->packTemperature(cid-12,x);
        }
    }
}

void BMS_StackInfo::addBattery(BMS_BatteryInfo *battery)
{
    this->m_batteries.append(battery);
}

void BMS_StackInfo::setHVC(BMS_HVCInfo *hvc)
{
    m_hvcInfo = hvc;
}


/****** BMS_SystemInfo **********/
BMS_SystemInfo::BMS_SystemInfo()
{

}

void BMS_SystemInfo::SetStackInfo(QList<BMS_StackInfo*> info)
{
    m_stacks = info;
}

void BMS_SystemInfo::AddStack(BMS_StackInfo *stack)
{
    m_stacks.append(stack);
}

void BMS_SystemInfo::ClearStack()
{
    m_stacks.clear();
}

BMS_StackInfo* BMS_SystemInfo::findStack(QString stackName)
{
    BMS_StackInfo *stack = nullptr;
    foreach(BMS_StackInfo *s, m_stacks){
        if(s->alias() == stackName){
            stack = s;
        }
    }
    return stack;
}

bool BMS_SystemInfo::Configuration(QByteArray data)
{
    bool ret = false;
    QJsonParseError e;
    QJsonDocument d = QJsonDocument::fromJson(data,&e);
    if(d.isNull()){
        return false;
    }
    QJsonObject obj = d.object();
    QJsonObject bal = obj["balancing"].toObject();
    if(obj.contains("stacks")){
        QJsonArray ja = obj["stacks"].toArray();
        int cells = 0,ntcs = 0;
        foreach(const QJsonValue &value,ja){
            QJsonObject o = value.toObject();
            cells = o["cells"].toInt();
            ntcs = o["NTC"].toInt();
            BMS_StackInfo *stack = new BMS_StackInfo();
            stack->groupID(o["group id"].toInt());
            stack->alias(o["name"].toString());
            for(int i=0;i<o["batteries"].toInt();i++){
                BMS_BatteryInfo *bat = new BMS_BatteryInfo(cells,ntcs);
                bat->balancingVoltage(bal["voltage"].toInt());
                bat->balancingHystersis(bal["hystersis"].toInt());
                bat->balancingONTime(bal["on time"].toInt());
                bat->balancingOFFTime(bal["off time"].toInt());
                stack->addBattery(bat);
            }
        }
    }

    return ret;
}

QByteArray BMS_SystemInfo::Configuration()
{
    QByteArray ret;

    return ret;
}

void BMS_SystemInfo::feedData(int msgid, QByteArray data)
{

}


/***** BMS_BatteryModel ****************/


BMS_BatteryModel::BMS_BatteryModel(QObject *parent):
    QAbstractTableModel(parent)
{
    for(int i=0;i<12;i++){
        m_header << QString("Cell\n%1").arg(i);
    }
    for(int i=0;i<5;i++){
        m_header << QString("NTC_%1").arg(i);
    }
    m_header << "Voltage SUM";
    m_activeStack = nullptr;
}

BMS_BatteryModel::~BMS_BatteryModel()
{

}

int BMS_BatteryModel::rowCount(const QModelIndex &parent) const
{
    if(m_activeStack != nullptr){
        return m_activeStack->BatteryCount();
    }
    return 0;
}

int BMS_BatteryModel::columnCount(const QModelIndex &parent) const
{
    return m_header.size();
}

QVariant BMS_BatteryModel::data(const QModelIndex &index, int role) const
{
    if(m_activeStack == nullptr) return QVariant();
    int row = index.row();
    int col = index.column();
    switch(role){
    case Qt::DisplayRole:
    case Qt::EditRole:
        return m_activeStack->queueData(row,col);
    case Qt::FontRole:
        break;
    case Qt::BackgroundRole:
        if(m_activeStack->queueData(row,col+17)==1){
            return QVariant((QColor(Qt::red)));
        }
        else{
            return QVariant();
        }
    case Qt::TextAlignmentRole:
        return (Qt::AlignVCenter);
    case Qt::CheckStateRole:
        break;
    }
    return QVariant();
}

QVariant BMS_BatteryModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(role == Qt::DisplayRole && orientation == Qt::Horizontal){
        return QString(m_header.at(section));
    }
    else if(role == Qt::DisplayRole && orientation == Qt::Vertical){
        return QString("ID= %1").arg(section+1);
    }
    else if(role == Qt::SizeHintRole && orientation == Qt::Horizontal){
        return QSize(60 ,25);
    }
    return QVariant();
}

//bool BMS_BatteryModel::setData(const QModelIndex &index, const QVariant &value, int role)
//{
//    return false;
//}

Qt::ItemFlags BMS_BatteryModel::flags(const QModelIndex &index) const
{
    return QAbstractTableModel::flags(index);
}

//bool BMS_BatteryModel::insertRows(int row, int count, const QModelIndex &parent)
//{
//    return true;
//}

//bool BMS_BatteryModel::removeRows(int row, int count, const QModelIndex &parent)
//{
//    return true;
//}

void BMS_BatteryModel::setStack(BMS_StackInfo *stack)
{
//    this->m_stacks.append(stack);
    this->m_activeStack = stack;
    //emit dataChanged();
//    emit dataChanged(QModelIndex(0,0),QModelIndex(0,0),)
}


/******* BMS_StackModel **********/

BMS_StackModel::BMS_StackModel(QObject *parent):
    QAbstractTableModel (parent)
{
    //m_header<<"電量狀態(SOC)"<<"總電壓(V)"<<"總電流(A)"<<"最高溫電池(ID/溫度)" << "最低溫電池(ID/溫度)" << "最大壓差(mV)";
    m_header = BMS_StackInfo::headerInfo();
}

BMS_StackModel::~BMS_StackModel()
{

}

int BMS_StackModel::rowCount(const QModelIndex &parent) const
{
    return m_stacks.size();
}

int BMS_StackModel::columnCount(const QModelIndex &parent) const
{
    return m_header.size();
}

QVariant BMS_StackModel::data(const QModelIndex &index, int role) const
{
    int row = index.row();
    int col = index.column();

    switch(role){
    case Qt::DisplayRole:
    case Qt::EditRole:
        switch(col){
        //case 0: return m_currentStack->
        }

        return "123";
    case Qt::TextAlignmentRole:
        return Qt::AlignRight;
    default: return QVariant();
    }
}

QVariant BMS_StackModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(role == Qt::DisplayRole){
        if(orientation == Qt::Horizontal){
            return m_header.at(section);
        }
        else if(orientation == Qt::Vertical){
            return QString("Stack:%1").arg(section+1);
        }
    }
    return QVariant();
}

Qt::ItemFlags BMS_StackModel::flags(const QModelIndex &index) const
{
    return QAbstractTableModel::flags(index);
}

void BMS_StackModel::addStack(BMS_StackInfo *stack)
{
    m_stacks.append(stack);
    m_currentStack = m_stacks.last();
}

void BMS_StackModel::setCurrentStack(int index)
{
    if(index < m_stacks.size()){
        m_currentStack = m_stacks.at(index);
    }
}

BMS_StackInfo *BMS_StackModel::findStack(int id)
{
    if(id < m_stacks.size()){
        return m_stacks.at(id);
    }
    return nullptr;
}


