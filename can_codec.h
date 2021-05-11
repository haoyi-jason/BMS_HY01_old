#ifndef CAN_CODEC_H
#define CAN_CODEC_H

#include <QObject>
#include <QtCore>

const quint16 CAN_TX_HEARTBEAT = 0x99;
const quint16 CAN_TX_BALANCING = 0x132;
const quint16 CAN_TX_NTC_PARAM = 0x133;

const quint16 CAN_RX_MODULE_BOOT = 0x101;
const quint16 CAN_RX_CELL_VOLT_0 = 0x110;
const quint16 CAN_RX_CELL_VOLT_1 = 0x111;
const quint16 CAN_RX_CELL_VOLT_2 = 0x112;
const quint16 CAN_RX_MODULE_TEMP_0 = 0x113;
const quint16 CAN_RX_MODULE_TEMP_1 = 0x114;
const quint16 CAN_RX_BALANCING_QUEUE = 0x115;

class Can_codec
{
public:
    Can_codec();
    Can_codec(uint32_t extID, uint8_t rtr, uint8_t dlc, uint8_t *d);
    Can_codec(uint8_t devID, uint16_t cmd, uint8_t rtr, uint8_t *d, uint8_t sz);

    bool isRemote();
    uint8_t DLC();
    uint8_t ID();
    uint8_t ReadData(uint8_t *d);
    void sendTo(uint8_t id, bool rtr, uint16_t cmd, uint8_t *d);
};

class CellDef
{
public:
    CellDef();
    uint16_t voltage;

};

#define NOF_CELL_PER_MODULE 12
#define NOF_TEMP_PER_MODULE 5

class Battery_Def
{
public:
    Battery_Def(uint8_t id);
    void setCellVoltage(uint8_t id, uint16_t volt_mv);
    void parseData(uint16_t cmd, uint8_t *d);
    void setModuleTemp(uint8_t id, uint16_t temperature);
    uint16_t cellVoltageSum();
    uint16_t cellVoltage(uint8_t id);
    uint8_t maxVoltage(uint16_t *voltage);
    uint8_t minVoltage(uint16_t *voltage);

private:
    uint16_t _cellVoltage[NOF_CELL_PER_MODULE];
    uint16_t _moduleTemp[NOF_TEMP_PER_MODULE];
    uint8_t _balancingState[NOF_CELL_PER_MODULE];
    uint8_t _enabledCellQueue[NOF_CELL_PER_MODULE];
    uint8_t _cellState[NOF_CELL_PER_MODULE];

};

class BMS_Model:public QAbstractTableModel
{
    Q_OBJECT
public:
    BMS_Model(QObject *parent = nullptr);
    ~BMS_Model();
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section,Qt::Orientation orientation, int role) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;
    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;

    Battery_Def *findItem(QString name);

private:
    QList<Battery_Def> m_data;
};

#endif // CAN_CODEC_H
