#include <QColor>
#include<QJsonDocument>
#include<QJsonObject>
#include<QJsonValue>
#include<QJsonArray>
#include "bms_model.h"
#include "bms_bmudevice.h"
#include "bms_bcudevice.h"
#include "bms_svidevice.h"
#include "bms_stack.h"

/***** BMS_BatteryModel ****************/


BMS_BatteryModel::BMS_BatteryModel(QObject *parent):
    QAbstractTableModel(parent)
{
    for(int i=0;i<12;i++){
        m_header << QString("C_%1(mV)").arg(i+1);
    }
    for(int i=0;i<5;i++){
        m_header << QString("T_%1(%2C)").arg(i+1).arg(QChar(0xb0));
    }
    m_header << "BATV(mV)";
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
    if(m_activeStack == nullptr) return 0;
    int g1 = m_activeStack->batteries().at(0)->cellCount();
    int g2 = g1 + m_activeStack->batteries().at(0)->ntcCount();
    return g2+1;
}

QVariant BMS_BatteryModel::data(const QModelIndex &index, int role) const
{
    if(m_activeStack == nullptr) return QVariant();
    int row = index.row();
    int col = index.column();
    ushort v;
    switch(role){
    case Qt::DisplayRole:
    case Qt::EditRole:
        v = m_activeStack->queueData(row,col);
        if(col < m_activeStack->batteries().at(0)->cellCount()){
            return QVariant(v);
        }
        else if(col < (m_activeStack->batteries().at(0)->cellCount() + m_activeStack->batteries().at(0)->ntcCount())){
            QString disp = QString("%1").arg(v/10.,5,'f',2,'0');
            return QVariant(disp);
        }
        else{
            return QVariant(v);
        }
    case Qt::FontRole:
        break;
    case Qt::BackgroundRole:
    {
        BMS_BMUDevice *bmu = m_activeStack->batteries().at(row);
        if(bmu != nullptr){
            ushort cmp = (1<<col);
            ushort ow = bmu->openwireBit() & cmp;
            ushort bl = bmu->balancingBit() & cmp;
            if(ow == 0 && bl==0){
                return QVariant();
            }
            else if(ow != 0){
                return QVariant(QColor(Qt::yellow));
            }
            else if(bl != 0){
                return QVariant(QColor(Qt::green));
            }
        }
    }
    case Qt::TextAlignmentRole:
        return (Qt::AlignRight);
    case Qt::CheckStateRole:
        break;
    case Qt::ForegroundRole:
        BMS_BMUDevice *bmu = m_activeStack->batteries().at(row);
        if(bmu != nullptr){
            if(col < bmu->cellCount()){
                ushort cmp = (1 << col);
                ushort ov = bmu->ovSetMask();
                ushort uv = bmu->uvSetMask();
                if(ov & cmp){
                    return QVariant(QColor(Qt::red));
                }
                else if(uv & cmp){
                    return QVariant(QColor(Qt::blue));
                }
                else{
                    return QVariant();
                }
            }
        }
        return QVariant();
        break;
    }
    return QVariant();
}

QVariant BMS_BatteryModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(m_activeStack == nullptr) return QVariant();
    if(role == Qt::DisplayRole && orientation == Qt::Horizontal){
        int g1 = m_activeStack->batteries().at(0)->cellCount();
        int g2 = g1 + m_activeStack->batteries().at(0)->ntcCount();
        if(section < g1){
            return QVariant(QString("C%1(mV)").arg(section+1));
        }
        else if(section < g2){
            return QVariant(QString("T%1(%2C)").arg(section-g1+1).arg(QChar(0xb0)));
        }
        return QString("BAT_VOLT");
    }
    else if(role == Qt::DisplayRole && orientation == Qt::Vertical){
        int g = m_activeStack->groupID();
        QString str=QString("%1_%2").arg(g).arg(section+1);
        return str;
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

void BMS_BatteryModel::setStack(BMS_Stack *stack)
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
    m_header<<"電量狀態(SOC)"<<"總電壓(V)"<<"總電流(A)"<<"最高溫電池(ID/溫度)" << "最低溫電池(ID/溫度)" << "最大壓差(mV)";
    //m_header = BMS_Stack::headerInfo();
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
    BMS_Stack *s = m_stacks[row];
    switch(role){
    case Qt::DisplayRole:
    case Qt::EditRole:
        switch(col){
        case 0: return  QVariant(s->soc());break;
        case 1: return  QVariant(s->stackVoltage());break;
        case 2: return  QVariant(s->stackCurrent());break;
        case 3: return  QVariant(s->maxCellVoltage());break;
        case 4: return  QVariant(s->maxCellVoltIndex());break;
        case 5: return  QVariant(s->minCellVoltage());break;
        case 6: return  QVariant(s->minCellVoltIndex());break;
        case 7: return  QVariant(s->maxStackTemperature());break;
        case 8: return  QVariant(s->maxStackTemperature());break;
        case 9: return  QVariant(s->minStackTemperature());break;
        case 10: return QVariant(s->minStackTemperature());break;
        //case 11: return m_currentStack->m;break;
        default: return QVariant(0);
        }
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

void BMS_StackModel::addStack(BMS_Stack *stack)
{
    m_stacks.append(stack);
    m_activeStack = m_stacks.last();
}

void BMS_StackModel::setCurrentStack(int index)
{
    if(index < m_stacks.size()){
        m_activeStack = m_stacks.at(index);
    }
}

BMS_Stack *BMS_StackModel::findStack(int id)
{
    if(id < m_stacks.size()){
        return m_stacks.at(id);
    }
    return nullptr;
}



//*** BMS_EventModel ***/

BMS_EventModel::BMS_EventModel(QObject *parent):QAbstractTableModel(parent)
{
    m_header<<"ID"<<"EVID"<<"EVLEVEL"<<"ALARM?"<<"WARNING?"<<"DATE"<<"Description";
}

BMS_EventModel::~BMS_EventModel()
{

}

int BMS_EventModel::rowCount(const QModelIndex &parent) const
{
    return m_events.size();
}

int BMS_EventModel::columnCount(const QModelIndex &parent) const
{
    return m_header.size();
}

QVariant BMS_EventModel::data(const QModelIndex &index, int role) const
{
    if(m_events.size() == 0) return QVariant();
    int row = index.row();
    int col = index.column();

    BMS_Event *evt = m_events.at(row);

    switch(role){
    case Qt::DisplayRole:
    case Qt::EditRole:
        switch(col){
        case 0: return QVariant(row);break;
        case 1: return QVariant(evt->m_evtID);break;
        case 2: return QVariant(evt->m_evtLevel);break;
        case 3: return QVariant(evt->m_isAlarm?"TRUE":"FALSE");
        case 4: return QVariant(evt->m_isWarning?"TRUE":"FALSE");
        case 5: return QVariant(evt->m_timeStamp.toString("yyyy-MM-dd hhmmss"));
        case 6: return QVariant(evt->m_description);
        default: return QVariant();
        }
    case Qt::FontRole:
        break;
    case Qt::BackgroundRole:
        return QVariant();
//        if(m_activeStack->queueData(row,col+17)==1){
//            return QVariant((QColor(Qt::red)));
//        }
//        else{
//            return QVariant();
//        }
    case Qt::TextAlignmentRole:
        return (Qt::AlignVCenter);
    case Qt::CheckStateRole:
        break;
    case Qt::ForegroundRole:
        return QVariant();
//        if(m_activeStack->queueData(row,col)>3445){
//            return QVariant(QColor(Qt::green));
//        }
//        else if(m_activeStack->queueData(row,col)>3425){
//            return QVariant(QColor(Qt::blue));
//        }
//        else{
//            return QVariant();
//        }
        break;
    }
    return QVariant();
}

QVariant BMS_EventModel::headerData(int section, Qt::Orientation orientation, int role) const
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

Qt::ItemFlags BMS_EventModel::flags(const QModelIndex &index) const
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

void BMS_EventModel::appendEvent(BMS_Event *e)
{
    if(e != nullptr){
        m_events.append(e);
    }
}

