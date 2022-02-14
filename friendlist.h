#ifndef FRIENDLIST_H
#define FRIENDLIST_H

#include <map>
#include <QWidget>
#include <QHostAddress>
class QScrollArea;
class QVBoxLayout;
class QLsit;
class QPushButton;

class FriendList : public QWidget
{
    Q_OBJECT

public:
    FriendList(QWidget *parent = 0);
    ~FriendList();
    void add_item(QWidget *item); // 添加好友名片
    void sub_item(const QString &ip); // 按IP删除好友名片

private:
    QScrollArea *fndScroll; // 放面板的滚动区域
    QWidget *containWidget; // 放好友名片的面板
    QVBoxLayout *containLayout;
    QList<QWidget*> fList; // 保存已添加的好友名片的指针，删除时用
};

#endif // FRIENDLIST_H
