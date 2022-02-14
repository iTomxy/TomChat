#include <string>
#include "common.h"
#include "friendbutton.h"
#include "friendlist.h"
#include "itom.h"
#include <QByteArray>
#include <QDataStream>
#include <QHostInfo>
#include <QList>
#include <QMessageBox>
#include <QNetworkInterface>
#include <QString>
#include <QTcpServer>
#include <QTcpSocket>
#include <QUdpSocket>
#include <QVBoxLayout>
#include <QWidget>

iTom::iTom(const QString &uname, QWidget *parent) :
    userName(uname), QMainWindow(parent)
{
    gotBytes = fileBytes = nameSize = 0;
    file = Q_NULLPTR;
    ip = get_ip(); // 本机IP
    ipset.insert(ip_str2dig(ip)); // 添加本机IP进ipset

    // 好友列表
    friendList = new FriendList(this);
    resize(ITOM_SIZE);

    // 默认添加广播按钮
    FriendButton *fb = new FriendButton(
            QString("Broadcast"), // 好友名字
            QHostAddress("255.255.255.255"), // 好友IP（广播）
            userName, // 自己名字
            QHostAddress("255.255.255.255"), // 自己IP（也是广播）
            friendList);
    friendList->add_item((QWidget*)fb);
    fset.append(fb);
    ipset.insert(ip_str2dig("255.255.255.255")); // 添加广播地址进set

    QWidget *mainWidget = new QWidget(this);
    QVBoxLayout *mainLayout = new QVBoxLayout(mainWidget);

    // 退出按钮
    QPushButton *leaveBtn = new QPushButton(ip);
    connect(leaveBtn, SIGNAL(clicked()),
            this, SLOT(bye_world()));
    mainLayout->addWidget(friendList);
    mainLayout->addWidget(leaveBtn);
    mainWidget->setLayout(mainLayout);
    setCentralWidget(mainWidget);

    // TCP
    tServer = new QTcpServer(this);
    connect(tServer, SIGNAL(newConnection()),
            this, SLOT(accept_connect()));
    // 开始监听
    if(!tServer->listen(QHostAddress::Any, TCP_PORT))
        qDebug() << tServer->errorString();

    // UDP
    uSocket = new QUdpSocket(this);
    uSocket->bind(UDP_PORT, QUdpSocket::ShareAddress);
    connect(uSocket, SIGNAL(readyRead()),
            this, SLOT(read_datagram()));
    // 广播上线消息
    hello_world();
}

iTom::~iTom()
{
    delete friendList;
    delete tServer;
    delete uSocket;
}

/*--- 获取本机IP ---*/
QString iTom::get_ip()
{
    QString localhost = QHostInfo::localHostName();
    QHostInfo hostinfo = QHostInfo::fromName(localhost);
    QList<QHostAddress> al = hostinfo.addresses();
    foreach(QHostAddress ad, al)
        if(ad.protocol() == QAbstractSocket::IPv4Protocol)
            return ad.toString();
    return QString("255.255.255.255");
}

/*--- UDP 广播上线消息 ---*/
void iTom::hello_world()
{
    QByteArray buf;
    QDataStream out(&buf, QIODevice::WriteOnly);
    out.setVersion(DATA_STREAM_VERSION);
    out << (int)OnLine << userName << ip; // 名字 + IP
    uSocket->writeDatagram(buf.data(), buf.size(),
            QHostAddress("255.255.255.255"), UDP_PORT);
}

/*--- UDP 广播下线消息 ---*/
void iTom::bye_world()
{
    QByteArray buf;
    QDataStream out(&buf, QIODevice::WriteOnly);
    out.setVersion(DATA_STREAM_VERSION);
    out << (int)OffLine << ip; // 只发IP
    uSocket->writeDatagram(buf.data(), buf.size(),
                QHostAddress("255.255.255.255"), UDP_PORT);
    close(); // 关主界面
}

/*--- UDP 读广播报文 ---*/
void iTom::read_datagram()
{
    QByteArray buf;
    QDataStream ds(&buf, QIODevice::ReadWrite);
    ds.setVersion(DATA_STREAM_VERSION);
    FriendButton *fb = Q_NULLPTR;
    int type; // 信息类型
    quint32 peerIPdig; // IP地址压进一个整数
    QString peerName, peerIP, message;

    while(uSocket->hasPendingDatagrams())
    {
        buf.resize(uSocket->pendingDatagramSize());
        uSocket->readDatagram(buf.data(), buf.size());
        ds >> type;
        switch(type)
        {
        case OnLine: //*** 别人上线
            // 添加好友到列表中
            ds >> peerName >> peerIP; // 名字 + IP
            peerIPdig = ip_str2dig(peerIP);
            if(ipset.count(peerIPdig) > 0) // 已存在列表中
                break;
            else // 添加IP到ipset
                ipset.insert(peerIPdig);
            fb = new FriendButton(
                    peerName, QHostAddress(peerIP),
                    userName, QHostAddress(ip),
                    friendList);
            // 加进朋友列表
            friendList->add_item((QWidget*)fb);
            // 保存好友名片指针
            fset.append(fb);
            // 对上线信息的回复
            buf.clear();
            ds << (int)OnLine_Reply << userName << ip;
            uSocket->writeDatagram(buf.data(), buf.size(),
                        QHostAddress(peerIP), UDP_PORT);
            break;
        case OnLine_Reply: //*** 别人对自己上线的回复
            ds >> peerName >> peerIP; // 名字 + IP
            peerIPdig = ip_str2dig(peerIP);
            if(ipset.count(peerIPdig) > 0) // 已存在列表中
                break;
            else // 添加IP到ipset
                ipset.insert(peerIPdig);
            fb = new FriendButton(
                    peerName, QHostAddress(peerIP),
                    userName, QHostAddress(ip),
                    friendList);
            friendList->add_item((QWidget*)fb);
            fset.append(fb);
            break;
        case OffLine: //*** 好友下线
            ds >> peerIP; // 只接IP
            peerIPdig = ip_str2dig(peerIP);
            if(ipset.count(peerIPdig) < 1) // 本身不存在
                break;
            else
                ipset.erase(peerIPdig);
            // 从好友列表删除
            friendList->sub_item(peerIP);
            // 从指针集删除
            foreach(FriendButton *f, fset)
                if(ip_str2dig(f->peer_ip()) == peerIPdig)
                {
                    peerName = f->peer_name();
                    fset.removeAt(fset.indexOf(f));
                }
            QMessageBox::warning(this, "Friend Left",
                    QString("%1 Logout!").arg(peerName),
                    QMessageBox::Ok);
            break;
        case Message: //*** 信息
            ds >> peerName >> peerIP >> message; // 名字+IP+信息
            peerIPdig = ip_str2dig(peerIP);
            if(ipset.count(peerIPdig) <= 0)
            {
                fb = new FriendButton(
                        peerName, QHostAddress(peerIP),
                        userName, QHostAddress(ip),
                        friendList);
                friendList->add_item((QWidget*)fb);
                fset.append(fb);
                ipset.insert(peerIPdig);
            }
            // 发送源是自己（广播时）就不再显示
            if(peerName == userName)
                break;
            // 找相应的FriendButton转传文件
            foreach(FriendButton *f, fset)
                if(ip_str2dig(f->peer_ip()) == peerIPdig)
                {
                    f->relay_msg(message);
                    break;
                }
            break;
        } // end swhich
        buf.clear();
    } // end while
}

/*--- TCP 接受链接请求 ---*/
void iTom::accept_connect()
{
    tSocket = tServer->nextPendingConnection();
    connect(tSocket, SIGNAL(readyRead()),
            this, SLOT(receive_file()));
    connect(tSocket, SIGNAL(error(QAbstractSocket::SocketError)),
            this, SLOT(show_error(QAbstractSocket::SocketError)));
    gotBytes = 0;
    fileName.clear();
}

/*--- TCP 接收文件 ---*/
void iTom::receive_file()
{
    QDataStream in(tSocket);
    in.setVersion(DATA_STREAM_VERSION);
    // 首部未接收/未接收完
    if(gotBytes <= 2 * sizeof(qint64))
    {
        if(!nameSize) // 前两个长度字段未接收
        {
            if(tSocket->bytesAvailable() >= 2 * sizeof(qint64))
            {
                in >> fileBytes >> nameSize;
                gotBytes += 2 * sizeof(qint64);
            }
            else // 数据不足，等下次
               return;
        }
        else if(tSocket->bytesAvailable() >= nameSize)
        {
            in >> fileName;
            gotBytes += nameSize;
        }
        else // 数据不足文件名长度，等下次
            return;
    }

    // 已读文件名、文件未打开 -> 尝试打开文件
    if(!fileName.isEmpty() && file == Q_NULLPTR)
    {
        fileName = "C:/" + fileName;
        file = new QFile(fileName);
        if(!file->open(QFile::WriteOnly)) // 打开失败
        {
            delete file;
            file = Q_NULLPTR;
            return;
        }
    }
    // 文件未打开，不能进行后续操作
    if(file == Q_NULLPTR)
        return;
    // 文件未接收完
    if(gotBytes < fileBytes)
    {
        gotBytes += tSocket->bytesAvailable();
        file->write(tSocket->readAll());
    }
    // 文件已接收完
    if(gotBytes == fileBytes)
    {
        tSocket->close(); // 关socket
        file->close(); // 关文件
        delete file;
        file = Q_NULLPTR;
        gotBytes = fileBytes = nameSize = 0;
        fileName.clear();
        QMessageBox::warning(this, "File Received",
                        QString("%1 接收成功！").arg(fileName),
                        QMessageBox::Ok);
    }
}

/*--- TCP 出错处理 ---*/
void iTom::show_error(QAbstractSocket::SocketError)
{
    qDebug() << tSocket->errorString();
    tSocket->close(); // 关cocket
    tSocket = Q_NULLPTR;
    file = Q_NULLPTR;
    fileName.clear(); // 清空文件名
    fileBytes = gotBytes = nameSize = 0;
}

/*--- 断交处理 ---*/
void iTom::disconnect_slot()
{
    tSocket->close(); // 关cocket
    tSocket = Q_NULLPTR;
    file = Q_NULLPTR;
    fileName.clear(); // 清空文件名
    fileBytes = gotBytes = nameSize = 0;
}
