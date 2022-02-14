#include "common.h"
#include "friendbutton.h"
#include "friendlist.h"
#include <QScrollArea>
#include <QString>
#include <QVBoxLayout>

FriendList::FriendList(QWidget *parent) :
    QWidget(parent)
{
    containLayout = new QVBoxLayout;
    containLayout->setContentsMargins(0, 0, 0, 0);
    containLayout->setSpacing(0);
    // containWidget是放好友名片的地方
    // 其布局是垂直布局
    containWidget = new QWidget;
    containWidget->setLayout(containLayout);

    // containWidget被包含在一个ScrollArea里
    fndScroll = new QScrollArea(this);
    // 使得containWidget可随好友名片的增删而伸缩
    fndScroll->setWidgetResizable(true);
    fndScroll->setWidget(containWidget);

    QVBoxLayout *listLayout = new QVBoxLayout(this);
    listLayout->setContentsMargins(0, 0, 0, 0);
    listLayout->setSpacing(0);
    listLayout->addWidget(fndScroll);

    setLayout(listLayout);
}

FriendList::~FriendList()
{
    delete fndScroll;
    delete containWidget;
    delete containLayout;
}

/*--- 加好友 ---*/
void FriendList::add_item(QWidget *item)
{
    fList.append(item); // 保存指针
    int cnt = containLayout->count();
    if(cnt > 1)
        containLayout->removeItem(containLayout->itemAt(cnt - 1));
    containLayout->addWidget(item);
    containLayout->addStretch();
}

/*--- 按IP删好友 ---*/
void FriendList::sub_item(const QString &ip)
{
    FriendButton *fb;
    quint32 peerIPdig = ip_str2dig(ip);
    for(int i=fList.count()-1; i>=0; --i)
    {
        fb = (FriendButton*)fList[i];
        // 通过保存的好友名片指针
        // 找出它保存的对应好友的IP
        if(ip_str2dig(fb->peer_ip()) == peerIPdig)
        {
            fb->setParent(Q_NULLPTR);
            delete fb;
            fList.removeAt(i);
        }
    }
}
