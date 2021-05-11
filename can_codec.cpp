#include "can_codec.h"
/*
 * CAN_CODEC: canbus message codec class
 *
 */
Can_codec::Can_codec()
{

}

Can_codec::Can_codec(uint32_t extID, uint8_t rtr, uint8_t dlc, uint8_t *d)
{

}

Can_codec::Can_codec(uint8_t devID, uint16_t cmd, uint8_t rtr, uint8_t *d, uint8_t sz)
{

}

bool Can_codec::isRemote()
{

    return 0;
}

uint8_t Can_codec::DLC()
{
    return 0;
}

uint8_t Can_codec::ID()
{
    return 0;
}

uint8_t Can_codec::ReadData(uint8_t *d)
{
    return 0;
}

void Can_codec::sendTo(uint8_t id, bool rtr, uint16_t cmd, uint8_t *d)
{

}



// -- Cell Def
CellDef::CellDef(){

}

 //-- Battery_def

Battery_Def::Battery_Def(uint8_t id)
{

}

void Battery_Def::setCellVoltage(uint8_t id, uint16_t volt_mv)
{

}

void Battery_Def::parseData(uint16_t cmd, uint8_t *d)
{

}

void Battery_Def::setModuleTemp(uint8_t id, uint16_t temperature)
{

}

uint16_t Battery_Def::cellVoltageSum()
{
    return 0;
}

uint16_t Battery_Def::cellVoltage(uint8_t id)
{
    return 0;
}

uint8_t Battery_Def::maxVoltage(uint16_t *voltage)
{

    return 0;
}

uint8_t Battery_Def::minVoltage(uint16_t *voltage)
{
    return 0;
}


// -- BMS Model

BMS_Model::BMS_Model(QObject *parent):QAbstractTableModel(parent)
{

}

BMS_Model::~BMS_Model()
{

}

int BMS_Model::rowCount(const QModelIndex &parent) const
{

    return 0;
}

int BMS_Model::columnCount(const QModelIndex &parent) const
{

    return 0;
}

QVariant BMS_Model::data(const QModelIndex &index, int role) const
{
    return QVariant();
}

QVariant BMS_Model::headerData(int section, Qt::Orientation orientation,int role) const
{
    return QVariant();
}

bool BMS_Model::setData(const QModelIndex &index, const QVariant &value, int role)
{

    return true;
}

Qt::ItemFlags BMS_Model::flags(const QModelIndex &index) const
{

}

bool BMS_Model::insertRows(int row, int count, const QModelIndex & parent)
{

    return false;
}

bool BMS_Model::removeRows(int row, int count, const QModelIndex &parent)
{
    return false;
}

Battery_Def *BMS_Model::findItem(QString name)
{

    return nullptr;
}

