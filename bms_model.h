#ifndef BMS_MODEL_H
#define BMS_MODEL_H
#include <QObject>
#include <QtCore>
#include "bms_def.h"


class BMS_BatteryModel:public QAbstractTableModel
{
    Q_OBJECT

public:
    BMS_BatteryModel(QObject *parent = nullptr);
    ~BMS_BatteryModel();
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section,Qt::Orientation orientation, int role) const override;
//    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
//    bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;
//    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;

    void feedData(); // feed data for battery info

    void setStack(BMS_StackInfo *stack);
public slots:


signals:
    void editCompleted(const QString&);

private:
    //QList<BMS_StackInfo*> m_stacks;
    BMS_StackInfo *m_activeStack;
    QStringList m_header;
};


class BMS_StackModel:public QAbstractTableModel
{
    Q_OBJECT

public:
    BMS_StackModel(QObject *parent = nullptr);
    ~BMS_StackModel();
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section,Qt::Orientation orientation, int role) const override;
//    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
//    bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;
//    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;

    void feedData(); // feed data for battery info

    void setStack(BMS_StackInfo *stack);
    void addStack(BMS_StackInfo *stack);
    void setCurrentStack(int index);
    void setStack(QList<BMS_StackInfo*> stacks){
        m_stacks = stacks;
    }
    BMS_StackInfo *findStack(int id);
public slots:


signals:
    void editCompleted(const QString&);

private:
    QList<BMS_StackInfo*> m_stacks;
    BMS_StackInfo *m_activeStack;
    QStringList m_header;
};


class BMS_EventModel:public QAbstractTableModel
{
    Q_OBJECT

public:
    BMS_EventModel(QObject *parent = nullptr);
    ~BMS_EventModel();
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section,Qt::Orientation orientation, int role) const override;
//    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
//    bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;
//    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;

public slots:


signals:

private:
    QList<BMS_Event*> m_events;
    QStringList m_header;
};




#endif // BMS_MODEL_H
