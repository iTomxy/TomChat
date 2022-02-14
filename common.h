#ifndef COMMON_H
#define COMMON_H

#include <QDataStream>
#include <QSize>

/*------- Enumeration -------*/

// 信息/报文类型
enum MessageType
{
    OnLine = 1, // 上线
    OnLine_Reply = 2, // 上线消息的回复
    OffLine = 3, // 下线
    Message = 4 // 消息
};

/*------- Constants -------*/

// QDataStream的版本
const int DATA_STREAM_VERSION = QDataStream::Qt_5_8;

// UDP端口
const quint16 UDP_PORT = 3333;
// TCP端口
const quint16 TCP_PORT = 3334;
// 发文件每次发送的数据块大小: 4 kilo-byte
const qint64 LOAD_BYTES = 4 * 1024;

// 正方形头像边长
const int HEAD_LEN = 35;
// 好友名片/按钮最小尺寸
const QSize BTN_MIN_SIZE = QSize(100, 45);
// 主界面大小
const QSize ITOM_SIZE = QSize(250, 450);

/*------- Function -------*/

// IP压缩进32位整数
quint32 ip_str2dig(const QString &ipstr);

#endif // COMMON_H
