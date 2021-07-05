#ifndef BMS_LOCALCONFIG_H
#define BMS_LOCALCONFIG_H

#include <QObject>
#include <QJsonObject>

class BMS_MODBUSConfig{
public:
    QString Port;
    QString Bitrate;
    QString Parity;
    QString ID;
    bool Enable;
    bool EnableTCP;
    QString TCPPort;
    bool parseJson(QJsonObject o){
        if(o.contains("port")){
            Port = o["port"].toString();
        }
        if(o.contains("bitrate")){
            Bitrate = o["bitrate"].toString();
        }
        if(o.contains("parity")){
            Parity = o["parity"].toString();
        }
        if(o.contains("id")){
            ID = o["id"].toString();
        }
        if(o.contains("enable")){
            Enable = o["enable"].toBool();
        }
        if(o.contains("enable_tcp")){
            EnableTCP = o["enable_tcp"].toBool();
        }
        if(o.contains("tcp_port")){
            TCPPort = o["tcp_port"].toString();
        }
        return true;
    }
    void feedJson(QJsonObject *o){
        (*o)["port"] = Port;
        (*o)["bitrate"] = Bitrate;
        (*o)["parity"] = Parity;
        (*o)["id"] = ID;
        (*o)["enable"] = Enable;
        (*o)["enable_tcp"] = EnableTCP;
        (*o)["tcp_port"] = TCPPort;
    }
};

class BMS_BalancingConfig{
public:
    QString BalancingVolt;
    QString HystersisMV ;
    QString On_TimeSec ;
    QString Off_TimeSec;
    bool parseJson(QJsonObject o){
        if(o.contains("voltage")){
            BalancingVolt = o["voltage"].toString();
        }
        if(o.contains("hystersis")){
            HystersisMV = o["hystersis"].toString();
        }
        if(o.contains("on_time")){
            On_TimeSec = o["on_time"].toString();
        }
        if(o.contains("off_time")){
            Off_TimeSec = o["off_time"].toString();
        }
        return true;
    }
    void feedJson(QJsonObject *o){
        (*o)["voltage"]= BalancingVolt;
        (*o)["hystersis"] = HystersisMV;
        (*o)["on_time"] = On_TimeSec;
        (*o)["off_time"] = Off_TimeSec;
    }
};

class BMS_StackConfig{
public:
    QString StackCount ;
    QString BatteryPerStack ;
    QString CellPerBattery ;
    QString NTCPerBattery ;
    QString Capacity ;
    bool ConfigReady;
    bool parseJson(QJsonObject o){
        if(o.contains("count")){
            StackCount = o["count"].toString();
        }
        if(o.contains("baterries_per_stack")){
            BatteryPerStack = o["baterries_per_stack"].toString();
        }
        if(o.contains("cells_per_battery")){
            CellPerBattery = o["cells_per_battery"].toString();
        }
        if(o.contains("ntcs_per_stack")){
            NTCPerBattery = o["ntcs_per_stack"].toString();
        }
        if(o.contains("capacity")){
            Capacity = o["capacity"].toString();
        }
        ConfigReady = true;
        return true;
    }

    void feedJson(QJsonObject *o){
        (*o)["count"] = StackCount;
        (*o)["baterries_per_stack"] = BatteryPerStack;
        (*o)["cells_per_battery"] = CellPerBattery;
        (*o)["ntcs_per_stack"] = NTCPerBattery;
        (*o)["capacity"] = Capacity;
    }
};

class BCU_DigitalChannel{
public:
    qint32 Channels = 2;
    bool Inverted = false;
};

class BCU_AnalogChannel{
public:
    QString Channels ;
    QString Resolution ;
    QString FullScale ;
    QString Unit;
    bool parseJson(QJsonObject o){
        if(o.contains("channels")){
            Channels = o["channels"].toString();
        }
        if(o.contains("resolution")){
            Resolution = o["resolution"].toString();
        }
        if(o.contains("full_scale")){
            FullScale = o["full_scale"].toString();
        }
        if(o.contains("unit")){
            Unit = o["unit"].toString();
        }
        return true;
    }
    void feedJson(QJsonObject *o){
        (*o)["channels"] = Channels;
        (*o)["resolution"] = Resolution;
        (*o)["full_scale"] = FullScale;
        (*o)["unit"] = Unit;
    }
};

class BMS_BCUConfig{
public:
    BCU_DigitalChannel digital_input;
    BCU_DigitalChannel digital_output;
    BCU_AnalogChannel analog_input;

};

class BMS_CrieteriaSpec{
public:
    //QString Name;
    QString High_Set;
    QString High_Clr;
    QString Low_Set ;
    QString Low_Clr ;
    QString Duration;
    bool parseJson(QJsonObject o){
        if(o.contains("high_set")){
            High_Set = o["high_set"].toString();
        }
        if(o.contains("high_clr")){
            High_Clr = o["high_clr"].toString();
        }
        if(o.contains("low_set")){
            Low_Set = o["low_set"].toString();
        }
        if(o.contains("low_clr")){
            Low_Clr = o["low_clr"].toString();
        }
        if(o.contains("duration")){
            Duration = o["duration"].toString();
        }
        return true;
    }
    void feedJson(QJsonObject *o){
//        (*o)["high_set"] = QString("%1").arg(High_Set,8,'f',3);
//        (*o)["high_clr"] = QString("%1").arg(High_Clr,8,'f',3);
//        (*o)["low_set"] = QString("%1").arg(Low_Set,8,'f',3);
//        (*o)["low_clr"] = QString("%1").arg(Low_Clr,8,'f',3);
        (*o)["high_set"] = High_Set;
        (*o)["high_clr"] = High_Clr;
        (*o)["low_set"] = Low_Set;
        (*o)["low_clr"] = Low_Clr;
        (*o)["duration"] = Duration;
    }

};

class BMS_CriteriaConfig{
public:
    BMS_CrieteriaSpec volt_warning;
    BMS_CrieteriaSpec volt_alarm;
    BMS_CrieteriaSpec temp_warning;
    BMS_CrieteriaSpec temp_alarm;
    bool ConfigReady = false;
    bool parseJson(QJsonObject o){
        if(o.contains("volt-warning")){
            QJsonObject ob = o["volt-warning"].toObject();
            volt_warning.parseJson(ob);
        }
        if(o.contains("volt-alarm")){
            QJsonObject ob = o["volt-alarm"].toObject();
            volt_alarm.parseJson(ob);
        }
        if(o.contains("temp-warning")){
            QJsonObject ob = o["temp-warning"].toObject();
            temp_warning.parseJson(ob);
        }
        if(o.contains("temp-alarm")){
            QJsonObject ob = o["temp-alarm"].toObject();
            temp_alarm.parseJson(ob);
        }
        ConfigReady = true;
        return true;
    }

    void feedJson(QJsonObject *o){
        QJsonObject *ob;
//        if(o->contains("volt-warning")){
            //QJsonObject ob = o->value("volt-warning").toObject();

            ob = new QJsonObject;
            volt_warning.feedJson(ob);
            (*o)["volt-warning"] = *ob;
//        }
//        if(o->contains("volt-alarm")){
        //    QJsonObject ob = o->value("volt-alarm").toObject();
            ob = new QJsonObject;
            volt_alarm.feedJson(ob);
            (*o)["volt-alarm"] = *ob;
//        }
//        if(o->contains("temp-warning")){
//            QJsonObject ob = o->value("temp-warning").toObject();
            ob = new QJsonObject;
            temp_warning.feedJson(ob);
            (*o)["temp-warning"] = *ob;
//        }
//        if(o->contains("volt-alarm")){
//            QJsonObject ob = o->value("volt-alarm").toObject();
            ob = new QJsonObject;
            temp_alarm.feedJson(ob);
            (*o)["temp-alarm"] = *ob;
//        }
    }
};

class BMS_CriteriaSOC{
public:
    BMS_CrieteriaSpec warning;
    BMS_CrieteriaSpec alarm;
    bool ConfigReady = false;
    bool parseJson(QJsonObject o){
        if(o.contains("warning")){
            QJsonObject ob = o["warning"].toObject();
            warning.parseJson(ob);
        }
        if(o.contains("alarm")){
            QJsonObject ob = o["alarm"].toObject();
            alarm.parseJson(ob);
        }
        ConfigReady = true;
        return true;
    }

    void feedJson(QJsonObject *o){
        QJsonObject *ob;
//        if(o->contains("warning")){
//            QJsonObject ob = o->value("warning").toObject();
        ob = new QJsonObject;
            warning.feedJson(ob);
            (*o)["warning"] = *ob;
//        }
//        if(o->contains("alarm")){
            //QJsonObject ob = o->value("alarm").toObject();
            ob = new QJsonObject;
            alarm.feedJson(ob);
            (*o)["alarm"] = *ob;
//        }
    }

};


class BMS_SystemCriteriaConfig{
public:
    BMS_CriteriaConfig cell;
    BMS_CriteriaConfig stack;
    BMS_CriteriaSOC soc;
    bool ConfigReady = false;
    bool parseJson(QJsonObject o){
        if(o.contains("cell")){
            QJsonObject ob = o["cell"].toObject();
            cell.parseJson(ob);
        }
        if(o.contains("stack")){
            QJsonObject ob = o["stack"].toObject();
            stack.parseJson(ob);
        }
        if(o.contains("soc")){
            QJsonObject ob = o["soc"].toObject();
            soc.parseJson(ob);
        }
        ConfigReady = true;
        return true;
    }

    void feedJson(QJsonObject *o){
        QJsonObject *ob = new QJsonObject;
        //if(o->contains("cell")){
            //QJsonObject ob = o->value("cell").toObject();
            cell.feedJson(ob);
            (*o)["cell"] = *ob;
        //}
        //if(o->contains("stack")){
            //QJsonObject ob = o->value("stack").toObject();
            ob = new QJsonObject;
            stack.feedJson(ob);
            (*o)["stack"] = *ob;
        //}
        //if(o->contains("soc")){
            //QJsonObject ob = o->value("soc").toObject();
            ob = new QJsonObject;
            soc.feedJson(ob);
            (*o)["soc"] = *ob;
        //}
    }
};

class BMS_EventConfig{
public:
    QString warning_out;
    QString alarm_out;
    bool warning_latch = false;
    bool alarm_latch = false;
    bool ConfigReady = false;
    bool parseJson(QJsonObject o)
    {
        if(o.contains("warning_out")){
            warning_out = o["warning_out"].toString().trimmed();
        }
        if(o.contains("alarm_out")){
            alarm_out = o["alarm_out"].toString().trimmed();
        }
        if(o.contains("warning_latch")){
            warning_latch = o["warning_latch"].toBool();
        }
        if(o.contains("alarm_latch")){
            alarm_latch = o["alarm_latch"].toBool();
        }
        ConfigReady = true;
        return true;
    }
    void feedJson(QJsonObject *o){
        (*o)["warning_out"] = warning_out;
        (*o)["alarm_out"] = alarm_out;
        (*o)["warning_latch"] = warning_latch;
        (*o)["alarm_latch"] = alarm_latch;

    }
};

class BMS_RecordConfig{
public:
    bool EnableLog;
    bool EnableEventLog;
    QString LogDays;
    QString LogRecords;
    QString LogEventCount;
    QString LogPath;
    bool ConfigReady = false;
    bool parseJson(QJsonObject o){
        if(o.contains("log_days"))
            LogDays = o["log_days"].toString().trimmed();
        if(o.contains("log_path"))
            LogPath = o["log_path"].toString().trimmed();
        if(o.contains("log_records"))
            LogRecords = o["log_records"].toString().trimmed();
        if(o.contains("event_log_count"))
            LogEventCount = o["event_log_count"].toString().trimmed();
        if(o.contains("enableLog"))
            EnableLog = o["enableLog"].toBool();
        if(o.contains("enableEventLog"))
            EnableEventLog = o["enableEventLog"].toBool();
        ConfigReady = true;
        return true;
    }

    void feedJson(QJsonObject *o){
        (*o)["log_days"] = LogDays;
        (*o)["log_path"] = LogPath;
        (*o)["log_records"] = LogRecords;
        (*o)["event_log_count"] = LogEventCount;
        (*o)["enableLog"] = EnableLog;
        (*o)["enableEventLog"] = EnableEventLog;
    }


};

class BMS_SystemConfig{
public:
    QString Alias;
    QString ValidInterval;
    QString HostIP;
    QString ListenPort;
    bool Simulate;
    bool ConfigReady = false;
    bool parseJson(QJsonObject o){
        if(o.contains("simulate"))
            Simulate = o["simulate"].toBool();
        if(o.contains("valid_interval"))
            ValidInterval = o["valid_interval"].toString().trimmed();
        if(o.contains("alias"))
            Alias = o["alias"].toString().trimmed();
        if(o.contains("host_ip"))
            HostIP = o["host_ip"].toString();
        if(o.contains("port"))
            ListenPort = o["port"].toString();
        ConfigReady = true;
        return true;
    }

    void feedJson(QJsonObject *o){
        (*o)["simulate"] = Simulate;
        (*o)["valid_interval"] = ValidInterval;
        (*o)["alias"] = Alias;
        (*o)["host_ip"] = HostIP;
        (*o)["port"]=ListenPort;
    }
};

class BMS_NetworkConfig{
public:
    bool Enable;
    bool Dhcp;
    QString ip;
    QString gateway;
    QString mask;
    bool ConfigReady = false;
    bool parseJson(QJsonObject o){
        if(o.contains("enabled")){
            Enable = o["enabled"].toBool();
        }
        if(o.contains("dhcp"))
            Dhcp = o["dhcp"].toBool();
        if(o.contains("ip"))
            ip = o["ip"].toString().trimmed();
        if(o.contains("gateway"))
            gateway = o["gateway"].toString().trimmed();
        if(o.contains("net_mask"))
            mask = o["net_mask"].toString().trimmed();
        ConfigReady = true;
        return true;
    }
    void feedJson(QJsonObject *o){
        (*o)["enabled"] = Enable;
        (*o)["dhcp"] = Dhcp;
        (*o)["ip"] = ip;
        (*o)["gateway"] = gateway;
        (*o)["net_mask"] = mask;
    }
};

class BMS_LocalConfig : public QObject
{
    Q_OBJECT
public:
    BMS_SystemConfig system;
    BMS_BalancingConfig balancing;
    BMS_StackConfig stack;
    BMS_BCUConfig bcu;
    BMS_SystemCriteriaConfig criteria;
    BMS_RecordConfig record;
    BMS_MODBUSConfig modbus;
    BMS_NetworkConfig network;
    BMS_EventConfig event_output;

    explicit BMS_LocalConfig(QObject *parent = nullptr);
    void load(QString fileName);
    void load(QByteArray b);
    void save(QString fileName);
    void genDefault(QString fileName);

signals:

public slots:

private:
};

#endif // BMS_LOCALCONFIG_H
