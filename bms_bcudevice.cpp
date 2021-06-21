#include "bms_bcudevice.h"

BMS_BCUDevice::BMS_BCUDevice(QObject *parent) : QObject(parent)
{
    m_lastSeen = QDateTime::currentMSecsSinceEpoch();

}

void BMS_BCUDevice::add_digital_input(int n, bool inverted)
{
    for(int i=0;i<n;i++){
        HW_IOChannel *c = new HW_IOChannel();
        m_digitalInput.append(c);
    }
}

void BMS_BCUDevice::add_digital_output(int n, bool inverted)
{
    for(int i=0;i<n;i++){
        HW_IOChannel *c = new HW_IOChannel();
        m_digitalOutput.append(c);
    }
}

void BMS_BCUDevice::add_analog_input(int n){
    for(int i=0;i<n;i++){
        HW_IOChannel *c = new HW_IOChannel();
        m_analogInput.append(c);
    }
}

void BMS_BCUDevice::add_voltage_source(int n)
{
    for(int i=0;i<n;i++){
        HW_IOChannel *c = new HW_IOChannel();
        m_voltageSource.append(c);
    }
}

void BMS_BCUDevice::add_pwm_in(int n)
{
    for(int i=0;i<n;i++){
        HW_IOChannel *c = new HW_IOChannel();
        m_pwmInput.append(c);
    }
}

    int BMS_BCUDevice::lastSeen(){return QDateTime::currentMSecsSinceEpoch() - m_lastSeen;}

    QByteArray BMS_BCUDevice::getDigitalInput(){
        int index=0;
        QByteArray ret;
        quint8 b = 0x0;
        foreach (HW_IOChannel *c, m_digitalInput) {
           b |= (c->value()<<index);
           index++;
        }
        ret.append(b);
        return ret;
    }

    int BMS_BCUDevice::digitalOutState(int id)
    {
        if(id < m_digitalOutput.size()){
            return m_digitalOutput[id]->value();
        }
        return -1;
    }

    QByteArray BMS_BCUDevice::getDigitalOutput(){
        int index=0;
        QByteArray ret;
        quint8 b = 0x0;
        foreach (HW_IOChannel *c, m_digitalOutput) {
           b |= (c->value()<<index);
           index++;
        }
        ret.append(b);
        return ret;
    }

    QList<int> BMS_BCUDevice::getWorkingCurrent(){
        QList<int> ret;
        foreach (HW_IOChannel *c, m_voltageSource) {
            ret.append(c->value());
        }
        return ret;
    }

    CAN_Packet* BMS_BCUDevice::setDigitalOut(int id, int state){
        CAN_Packet *ret = nullptr;
        if(id < m_digitalOutput.size()){
            m_digitalOutput[id]->writeValue(state);
            if(m_digitalOutput[id]->valid_write()){
                ret = new CAN_Packet();
                ret->Command = 0x1140;
                quint8 b = 0x0;
                for(int i=0;i<m_digitalOutput.size();i++){
                    b |= (m_digitalOutput[i]->writeValue() << i);
                }
                ret->data.append((quint8)0x0); // output control
                ret->data.append(b);
            }
            if(m_simulating){
                m_digitalOutput[id]->value(state);
            }
        }
        return ret;
    }

    CAN_Packet* BMS_BCUDevice::setVoltageSource(int id, int currentMa, bool enable){
        CAN_Packet *ret = nullptr;
        if(id < m_voltageSource.size()){
            m_voltageSource[id]->writeValue(currentMa);

            if(m_voltageSource[id]->valid_write()){
                ret = new CAN_Packet();
                ret->Command = 0x1180;
                QDataStream ds(&ret->data,QIODevice::WriteOnly);
                ds.setByteOrder(QDataStream::LittleEndian);
                quint16 v;
                quint8 bv = (quint8)id;
                ds << bv;
                bv = enable?1:0;
                ds << bv;
                v = (quint16)m_voltageSource[id]->writeValue();
                ds << v;
            }
        }
        return ret;
    }

    CAN_Packet* BMS_BCUDevice::setADCRawLow(int id, int value){
        CAN_Packet *ret = nullptr;
        if(id < 8){
            ret = new CAN_Packet();
            ret->Command = 0x1160;
            QDataStream ds(&ret->data,QIODevice::WriteOnly);
            ds.setByteOrder(QDataStream::LittleEndian);
            quint16 v;
            quint8 bv = (quint8)id;
            ds << bv;
            bv = 0x0; // raw low
            ds << bv;
            v = (quint16)value;
            ds << v;
        }
        return ret;
    }

    CAN_Packet* BMS_BCUDevice::setADCRawHigh(int id, int value){
        CAN_Packet *ret = nullptr;
        if(id < 8){
            ret = new CAN_Packet();
            ret->Command = 0x1160;
            QDataStream ds(&ret->data,QIODevice::WriteOnly);
            ds.setByteOrder(QDataStream::LittleEndian);
            quint16 v;
            quint8 bv = (quint8)id;
            ds << bv;
            bv = 0x1; // raw high
            ds << bv;
            v = (quint16)value;
            ds << v;
        }
        return ret;
    }

    CAN_Packet* BMS_BCUDevice::setADCEngLow(int id, float value){
        CAN_Packet *ret = nullptr;
        if(id < 8){
            ret = new CAN_Packet();
            ret->Command = 0x1160;
            QDataStream ds(&ret->data,QIODevice::WriteOnly);
            ds.setByteOrder(QDataStream::LittleEndian);
            ds.setFloatingPointPrecision(QDataStream::SinglePrecision);
            float v;
            quint8 bv = (quint8)id;
            ds << bv;
            bv = 0x2; // eng low
            ds << bv;
            v = value;
            ds << v;
        }
        return ret;
    }

    CAN_Packet* BMS_BCUDevice::setADCEngHigh(int id, float value){
        CAN_Packet *ret = nullptr;
        if(id < 8){
            ret = new CAN_Packet();
            ret->Command = 0x1160;
            QDataStream ds(&ret->data,QIODevice::WriteOnly);
            ds.setByteOrder(QDataStream::LittleEndian);
            ds.setFloatingPointPrecision(QDataStream::SinglePrecision);
            float v;
            quint8 bv = (quint8)id;
            ds << bv;
            bv = 0x3; // eng high
            ds << bv;
            v = value;
            ds << v;
        }
        return ret;
    }

    CAN_Packet* BMS_BCUDevice::saveParam(){
        CAN_Packet *ret = new CAN_Packet();
        ret->Command = 0x160;
        ret->data[0] = 0x99;
        return ret;
    }
    void BMS_BCUDevice::generatePacket(){
        // check digital input
        bool gen = false;
        foreach (HW_IOChannel *c, m_digitalOutput) {
            if(!c->valid()) gen = true;
        }
        if(gen){
            CAN_Packet *p = new CAN_Packet();
            p->Command = 0x140;
            quint8 b = 0x0;
            for(int i=0;i<m_digitalOutput.size();i++){
                b |= (m_digitalOutput[i]->value() << i);
            }
            p->data.append(b);
            m_pendingAction.append(p);
        }

        gen = false;
        foreach (HW_IOChannel *c,m_voltageSource) {
            if(c->valid_write()) gen = true;
        }
        if(gen){
            CAN_Packet *p = new CAN_Packet();
            p->Command = 0x180;
            QDataStream ds(&p->data,QIODevice::WriteOnly);
            quint16 v;
            for(int i=0;i<m_voltageSource.size();i++){
                v = (quint16)m_voltageSource[i]->value();
                ds << v;
            }
            m_pendingAction.append(p);
        }
    }

    void BMS_BCUDevice::feedData(quint8 id, quint16 msg, QByteArray data){
        if(id == 0x01){ // BCU always occupied 0x01, group 0, id 1
            m_lastSeen = QDateTime::currentMSecsSinceEpoch();
            QDataStream ds(&data,QIODevice::ReadOnly);
            ds.setByteOrder(QDataStream::LittleEndian);
            quint16 uv;
            quint8 bv;
            switch(msg){
                case 0x121:
                    ds >> uv;
                    m_pwmInput[0]->value(uv);
                    ds >> uv;
                    m_pwmInput[1]->value(uv);
                    ds >> uv;
                    m_voltageSource[0]->value(uv);
                    ds >> uv;
                    m_voltageSource[1]->value(uv);
                    break;
                case 0x122:
                    ds >> bv;
                    for(int i=0;i<2;i++){
                        if((bv & (1 << i)) == (1 << i)){
                            m_digitalInput[i]->value(1);
                        }
                        else{
                            m_digitalInput[i]->value(0);
                        }
                    }
                    ds >> bv;
                    for(int i=0;i<2;i++){
                        if((bv & (1 << i)) == (1 << i)){
                            m_digitalOutput[i]->value(1);
                        }
                        else{
                            m_digitalOutput[i]->value(0);
                        }
                    }
                break;
                default:break;
            }
        }
    }

    int BMS_BCUDevice::isActionPending(){
//        int ret = NO_PENDING;
//        // check do first
//        foreach(HW_IOChannel *c,m_digitalOutput){
//            if(!c->valid()){
//                ret = DO_PENDING;
//            }
//        }
//        foreach (HW_IOChannel *c, m_analogInput) {
//            if(!c->valid()){
//                ret |= AO_PENDING;
//            }
//        }
//        return ret;
    }

//    QByteArray BMS_BCUDevice::genPacket(PendingAction action){
//        QByteArray ret;
////        switch(action){
////        case DO_PENDING:
////            ret.append(m_digitalOutput[0]->value() | (1<< m_digitalOutput[1]->value()));
////            break;
////        case AO_PENDING:
////            break;
////        default:break;
////        }

//        return ret;
//    }

    QDataStream &operator<<(QDataStream &out, const BMS_BCUDevice *dev)
    {
        quint8 bv = 0;
        out << dev->m_lastSeen;
        bv = (quint8)dev->m_digitalInput.size();
        out << bv;
        bv = (quint8)dev->m_digitalOutput.size();
        out << bv;
        bv = (quint8)dev->m_analogInput.size();
        out << bv;
        bv = (quint8)dev->m_pwmInput.size();
        out << bv;
        bv = (quint8)dev->m_voltageSource.size();
        out << bv;

        int index=0;
        bv = 0;
        foreach(HW_IOChannel *c, dev->m_digitalInput){
            if(c->value() != 0){
                bv |= (1 << index);
            }
            index++;
            if(index == 8){
                index = 0;
            }
        }
        out << bv;
        bv = 0;
        index = 0;
        foreach(HW_IOChannel *c, dev->m_digitalOutput){
            if(c->value() != 0){
                bv |= (1 << index);
            }
            index++;
            if(index == 8){
                index = 0;
            }
        }
        out << bv;

        if(dev->m_analogInput.size() > 0){
            foreach(HW_IOChannel *c, dev->m_analogInput){
                out << c->value();
            }
        }
        if(dev->m_pwmInput.size() > 0){
            foreach(HW_IOChannel *c, dev->m_pwmInput){
                out << c->value();
            }
        }
        if(dev->m_voltageSource.size() > 0){
            foreach(HW_IOChannel *c, dev->m_voltageSource){
                out << c->value();
            }
        }
        return out;
    }

    QDataStream &operator >> (QDataStream &in, BMS_BCUDevice *dev)
    {
        quint8 bv;
        int v;
        in >> dev->m_lastSeen;
        in >> bv;
        if(dev->m_digitalInput.size() == 0){
            for(int i=0;i<bv;i++){
                dev->m_digitalInput.append(new HW_IOChannel());
            }
        }
        in >> bv;
        if(dev->m_digitalOutput.size() == 0){
            for(int i=0;i<bv;i++){
                dev->m_digitalOutput.append(new HW_IOChannel());
            }
        }
        in >> bv;
        if(dev->m_analogInput.size() == 0){
            for(int i=0;i<bv;i++){
                dev->m_analogInput.append(new HW_IOChannel());
            }
        }
        in >> bv;
        if(dev->m_pwmInput.size() == 0){
            for(int i=0;i<bv;i++){
                dev->m_pwmInput.append(new HW_IOChannel());
            }
        }
        in >> bv;
        if(dev->m_voltageSource.size() == 0){
            for(int i=0;i<bv;i++){
                dev->m_voltageSource.append(new HW_IOChannel());
            }
        }

        if(dev->m_digitalInput.size() > 0){
            in >> bv;
            dev->m_digitalInput[0]->value((bv&0x1)?1:0);
            dev->m_digitalInput[1]->value((bv&0x2)?1:0);
        }
        if(dev->m_digitalOutput.size() > 0){
            in >> bv;
            dev->m_digitalOutput[0]->value((bv&0x1)?1:0);
            dev->m_digitalOutput[1]->value((bv&0x2)?1:0);
        }

        if(dev->m_analogInput.size() > 0){
            foreach (HW_IOChannel *c, dev->m_analogInput) {
                in >> v;
                c->value(v);
            }
        }

        if(dev->m_pwmInput.size() > 0){
            foreach (HW_IOChannel *c, dev->m_pwmInput) {
                in >> v;
                c->value(v);
            }
        }

        if(dev->m_voltageSource.size() > 0){
            foreach (HW_IOChannel *c, dev->m_voltageSource) {
                in >> v;
                c->value(v);
            }
        }
        return in;
    }

    CAN_Packet* BMS_BCUDevice::ActionPending(){
        CAN_Packet *ret=nullptr;
        if(m_pendingAction.size() > 0){
            ret = m_pendingAction.first();
            m_pendingAction.removeFirst();
        }
        return ret;
    }

    int BMS_BCUDevice::vsource_limit(int id)
    {
        return m_voltageSource[id]->limit();
    }

    void BMS_BCUDevice::vsource_limit(int id, int value)
    {
        m_voltageSource[id]->limit(value);
    }

    void BMS_BCUDevice::simData()
    {
        m_lastSeen = QDateTime::currentMSecsSinceEpoch();
    }
