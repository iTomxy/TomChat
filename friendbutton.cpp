#include "common.h"
#include "chatwindow.h"
#include "friendbutton.h"
#include <QHostAddress>
#include <QIcon>
#include <QLabel>
#include <QMouseEvent>
#include <QPixmap>
#include <QString>
#include <QHBoxLayout>

FriendButton::FriendButton(
        const QString &pN, // 好友名字
        const QHostAddress &pA, // 好友IP
        const QString &mN, // 自己名字
        const QHostAddress &mA, // 自己IP
        QWidget *p) :
    QPushButton(p), peerName(pN), peerAddr(pA), myName(mN), myAddr(mA)
{
    // 水平布局，左头像右名字
    QHBoxLayout *hBox = new QHBoxLayout;

    // 头像
    QLabel *head = new QLabel(this);
    QIcon headIcon(QString(":/img/image/head.jpg"));
    QPixmap headPM= headIcon.pixmap(QSize(HEAD_LEN, HEAD_LEN));
    head->setPixmap(headPM);
    head->setFixedSize(HEAD_LEN, HEAD_LEN); // 固定大小
    // 名字
    QLabel *uname = new QLabel(peerName, this);

    hBox->addWidget(head);
    hBox->addWidget(uname);
    hBox->setContentsMargins(4,0,0,0);
    setLayout(hBox);

    // 设置按钮的最小尺寸
    setMinimumSize(BTN_MIN_SIZE);

    isOpen = false;
    cw = Q_NULLPTR;
}

FriendButton::~FriendButton()
{

}

/*--- 获取好友IP ---*/
QString FriendButton::peer_ip()
{
    return peerAddr.toString();
}

/*--- 获取好友名字 ---*/
QString FriendButton::peer_name()
{
    return peerName;
}

/*--- 单击弹出新聊天窗口 ---*/
void FriendButton::mousePressEvent(QMouseEvent *e)
{
    cw = new ChatWindow(peerName, peerAddr, myName, myAddr, this);
    connect(cw, SIGNAL(close_chat()),
            this, SLOT(window_close()));
    cw->show();
    isOpen = true;
}

/*--- 响应聊天窗口关闭 ---*/
void FriendButton::window_close()
{
    isOpen = false;
    cw = Q_NULLPTR;
}

/*--- 将信息传给聊天窗口 ---*/
void FriendButton::relay_msg(const QString &msg)
{
    // 聊天窗未打开 -> 新建聊天窗口
    if(!isOpen)
    {
        cw = new ChatWindow(peerName, peerAddr, myName, myAddr, this);
        connect(cw, SIGNAL(close_chat()),
                this, SLOT(window_close()));
        cw->show();
        isOpen = true;
    }
    // 聊天窗口接收信息
    cw->get_msg(msg);
}
