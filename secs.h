#ifndef SECS_H
#define SECS_H
#include <QObject>

namespace secs {
#define FORMAT_TYPE(x)  (x >> 2)
#define FORMAT_LENGTH(x)    (x & 0x03)
}

class hsmsParser:public QObject
{
    Q_OBJECT
public:
    enum HSMS_FORMAT{
        FORMAT_LIST = 0x0,
        FORMAT_BIN = 0x08,
        FORMAT_BOOL = 0x09,
        FORMAT_ASCII = 0x10,
        FORMAT_JIS8 = 0x11,
        FORMAT_CHAR2 = 0x12,
        FORMAT_INT8 = 0x18,
        FORMAT_INT1 = 0x19,
        FORMAT_INT2 = 0x1A,
        FORMAT_INT4 = 0x1B,
        FORMAT_F8 = 0x20,
        FORMAT_F4 = 0x24,
        FORMAT_UINT8 = 0x28,
        FORMAT_UINT1 = 0x29,
        FORMAT_UINT2 = 0x2A,
        FORMAT_UINT4 = 0x2B
    };
    Q_ENUM(HSMS_FORMAT)
    enum BMS_FORMAT{
        BMS_LIST = 0x0,
        BMS_CONFIG = 0x1,
        BMS_STACK = 0x2,
        BMS_CRITICAL = 0x3
    };
    Q_ENUM(BMS_FORMAT)
    hsmsParser(QObject *parent = nullptr);
    static quint8 getHeader(QByteArray b, int* len, quint8 *hlen){
        quint8 ret = b[0];
        quint8 lenBytes = ret & 0x3;
        ret >>= 2;
        int l = 0;
        for(int i=0;i<lenBytes;i++){
            l <<=8;
            l |= b[i+1];
        }
        *len = l;
        *hlen = lenBytes+1;
        return ret;
    }
    static QByteArray genHeader(BMS_FORMAT format, int size){
        QByteArray ret;
        quint8 b0 = size % 256;
        quint8 b1 = (size/256)%256;
        quint8 b2 = (size / 65536)%256;

        if(b2 == 0){
            if(b1 == 0){
                ret.append((format <<2) | 0x01);
                ret.append(b0);
            }
            else{
                ret.append((format <<2) | 0x02);
                ret.append(b0);
                ret.append(b1);
            }
        }else{
            ret.append((format <<2) | 0x03);
            ret.append(b0);
            ret.append(b1);
            ret.append(b2);
        }
        return ret;
    }

};

#endif // SECS_H
