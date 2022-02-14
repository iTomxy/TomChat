#ifndef ITOM_H
#define ITOM_H

#include <set>
#include "friendbutton.h"
#include <QAbstractSocket>
#include <QList>
#include <QMainWindow>
#include <QObject>
class FriendList;
class QFile;
class QHostAddress;
class QString;
class QTcpServer;
class QTcpSocket;
class QUdpSocket;

class iTom : public QMainWindow
{
    Q_OBJECT

public:
    iTom(
        const QString &uname, // 用户名
        QWidget *parent = 0
    );
    ~iTom();

private slots:
    void accept_connect(); // 接受链接请求
    void receive_file(); // 接收文件
    void show_error(QAbstractSocket::SocketError);
    void disconnect_slot();
    void hello_world(); // 广播上线信息
    void bye_world(); // 广播下线信息
    void read_datagram();

private:
    QString get_ip(); // 获取本机IP

private:
    FriendList *friendList; // 装FriendButton（好友名片）的
    QList<FriendButton*> fset; // 保存FriendButton指针，传递信息用
    QString userName, ip; // 用户昵称、本机IP
    std::set<quint32> ipset; // IP集合
    QTcpServer *tServer;
    QTcpSocket *tSocket;
    QUdpSocket *uSocket;
    // 接收文件用：已收字节数、总字节数、文件名长度
    qint64 gotBytes, fileBytes, nameSize;
    QFile *file;
    QString fileName;
};

#endif // ITOM_H
