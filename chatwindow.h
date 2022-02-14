#ifndef CHATWINDOW_H
#define CHATWINDOW_H

#include <QAbstractSocket>
#include <QHostAddress>
#include <QMainWindow>
#include <QCloseEvent>
class QFile;
class QString;
class QTcpSocket;
class QTextEdit;
class QUdpSocket;

namespace Ui {
    class ChatWindow;
}

class ChatWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit ChatWindow(
        const QString &pName, // 好友名字
        const QHostAddress &pAddr, // 好友IP
        const QString &mName, // 自己名字
        const QHostAddress &mAddr, // 自己IP
        QWidget *parent = 0
    );
    ~ChatWindow();
    void get_msg(const QString &msg); // 从主窗口获得信息

signals:
    void close_chat(); // 关闭窗口时触发

private slots:
    void on_msgBtn_clicked(); // 发送信息
    void on_fileBtn_clicked(); // 发送文件
    void start_transfer(); // 文件首部：总大小、文件名长度、文件名
    void continue_transfer(qint64); // 正式传文件
    void show_error(QAbstractSocket::SocketError);
    void closeEvent(QCloseEvent*); // 关闭窗口事件

private:
    Ui::ChatWindow *ui;
    QTextEdit *showEdit, *sendEdit; // 信息编辑框、已发送信息显示框

    QString peerName, myName; // 好友名字、自己名字
    QHostAddress peerAddr, myAddr; // 好友IP、自己IP

    QUdpSocket *uSocket;
    QTcpSocket *tSocket;

    QFile *file;
    QString fileName;
    qint64 fileBytes, sentBytes, restBytes, loadBytes;
};

#endif // CHATWINDOW_H
