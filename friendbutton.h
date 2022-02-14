#ifndef FRIENDBUTTON_H
#define FRIENDBUTTON_H

#include <QObject>
#include <QPushButton>
#include <QHostAddress>
class ChatWindow;
class QString;
class QMoustEvent;

class FriendButton : public QPushButton
{
    Q_OBJECT

public:
    FriendButton(
        const QString &pN, // 好友名字
        const QHostAddress &pA, // 好友IP
        const QString &mN, // 自己名字
        const QHostAddress &mA, // 自己IP
        QWidget *parent = 0
    );
    ~FriendButton();
    void relay_msg(const QString &msg); // 给聊天窗传递聊天信息
    QString peer_name(); // 获取好友名字
    QString peer_ip(); // 获取好友IP

protected:
    void mousePressEvent(QMouseEvent *e); // 弹出聊天框

private slots:
    void window_close();

private:
    QHostAddress peerAddr, myAddr; // 好友IP、自己IP
    QString peerName, myName; // 好友名字、自己名字
    bool isOpen; // 聊天窗是否已打开
    ChatWindow *cw; // 聊天窗口指针
};

#endif // FRIENDBUTTON_H
