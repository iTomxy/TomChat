#include "common.h"
#include "chatwindow.h"
#include "ui_chatwindow.h"
#include <QByteArray>
#include <QDataStream>
#include <QDateTime>
#include <QFile>
#include <QFileDialog>
#include <QHostAddress>
#include <QMessageBox>
#include <QString>
#include <QTcpSocket>
#include <QTextEdit>
#include <QUdpSocket>
#include <QVBoxLayout>
#include <QScrollBar>

ChatWindow::ChatWindow(
        const QString &pName, // 好友名字
        const QHostAddress &pAddr, // 好友IP
        const QString &mName, // 自己名字
        const QHostAddress &mAddr, // 自己IP
        QWidget *parent) :
    peerName(pName), peerAddr(pAddr),
    myName(mName), myAddr(mAddr),
    QMainWindow(parent), ui(new Ui::ChatWindow)
{
    ui->setupUi(this);
    fileBytes = sentBytes= restBytes = 0;
    loadBytes = LOAD_BYTES;
    file = Q_NULLPTR;

    // 广播不能发文件 -> 禁用发文件按钮
    if(peerAddr == QHostAddress::Broadcast)
        ui->fileBtn->setEnabled(false);

    // 信息显示框
    ui->showScroll->setWidgetResizable(true);
    showEdit = new QTextEdit(this);
    showEdit->setReadOnly(true);
    showEdit->setTextColor(Qt::black);
    QWidget *showWidget = new QWidget(ui->showScroll);
    QVBoxLayout *showVB = new QVBoxLayout(showWidget);
    showVB->addWidget(showEdit);
    showWidget->setLayout(showVB);
    ui->showScroll->setWidget(showWidget);

    // 信息编辑框
    ui->sendScroll->setWidgetResizable(true);
    QWidget *sendWidget = new QWidget(ui->sendScroll);
    QVBoxLayout *sendVB = new QVBoxLayout(sendWidget);
    sendEdit = new QTextEdit(sendWidget);
    sendVB->addWidget(sendEdit);
    sendWidget->setLayout(sendVB);
    ui->sendScroll->setWidget(sendWidget);

    // UDP Socket
    uSocket = new QUdpSocket(this);

    // TCP Socket
    tSocket = new QTcpSocket(this);
    // 连接已建立 -> 开始发文件
    connect(tSocket, SIGNAL(connected()),
            this, SLOT(start_transfer()));
    // 文件首部已发出 -> 继续发
    connect(tSocket, SIGNAL(bytesWritten(qint64)),
            this, SLOT(continue_transfer(qint64)));
    // socket出错 -> 错误处理
    connect(tSocket, SIGNAL(error(QAbstractSocket::SocketError)),
            this, SLOT(show_error(QAbstractSocket::SocketError)));
}

ChatWindow::~ChatWindow()
{
    delete ui;
}

/*--- 发送信息 ---*/
void ChatWindow::on_msgBtn_clicked()
{
    // 没东西
    if(sendEdit->toPlainText().isEmpty())
    {
        QMessageBox::warning(this, "Send Nothing",
            "There is NOTHING to send!", QMessageBox::Ok);
        return;
    }

    QByteArray buf;
    QDataStream out(&buf, QIODevice::WriteOnly);
    out.setVersion(DATA_STREAM_VERSION);

    // 发送：类型 + 名字 + IP + 信息
    out << (int)Message << myName << myAddr.toString()
        << sendEdit->toPlainText();
    uSocket->writeDatagram(buf.data(), buf.size(),
                           peerAddr, UDP_PORT);
    // 加进展示框
    QString time = QDateTime::currentDateTime().
                toString("yyyy-MM-dd hh:mm:ss");
    showEdit->append("Me " + time);
    // 发送内容加入到展示框
    showEdit->append(sendEdit->toPlainText());
    sendEdit->clear(); // 清空输入框
}

/*--- 接收信息 <- 来自主窗口 ---*/
void ChatWindow::get_msg(const QString &msg)
{
    // 加进展示框
    QString time = QDateTime::currentDateTime().
                toString("yyyy-MM-dd hh:mm:ss");
    showEdit->append(peerName + " " + time);
    showEdit->append(msg); // 发送内容加入到展示框
}

/*--- 选择发送文件 ---*/
void ChatWindow::on_fileBtn_clicked()
{
    fileName = QFileDialog::getOpenFileName(this);
    if(fileName.isEmpty())
    {
        QMessageBox::warning(this, "File Not Open",
            "选择文件错误!请重试...", QMessageBox::Ok);
        return;
    }
    // 发送连接请求
    tSocket->connectToHost(peerAddr, TCP_PORT);
    sentBytes = 0;
}

/*--- 开始传文件 -> 发送首部 ---*/
void ChatWindow::start_transfer()
{
    file = new QFile(fileName);
    if(!file->open(QFile::ReadOnly))
    {
        qDebug() << "*** start_transfer(): File Open Error";
        return;
    }

    fileBytes = file->size();

    QByteArray buf;
    QDataStream out(&buf, QIODevice::WriteOnly);
    out.setVersion(DATA_STREAM_VERSION);

    // 无路径文件名
    QString sfName = fileName.right(fileName.size() -
                fileName.lastIndexOf('/') - 1);
    // 首部 = 总大小 + 文件名长度 + 文件名
    out << qint64(0) << qint64(0) << sfName;
    // 总大小加上首部的大小
    fileBytes += buf.size();
    // 重写首部的前两个长度字段
    out.device()->seek(0);
    out << fileBytes << (qint64(buf.size()) - 2 * sizeof(qint64));
    // 发送首部，计算剩余大小
    restBytes = fileBytes - tSocket->write(buf);
}

/*--- 继续传文件 -> 文件本身 ---*/
void ChatWindow::continue_transfer(qint64 sentSize)
{
    sentBytes += sentSize;

    // 还有数据要发
    if(restBytes > 0)
    {
        // 从文件读数据
        QByteArray buf = file->read(qMin(loadBytes, restBytes));
        // 发送
        restBytes -= tSocket->write(buf);
    }
    else
        file->close();

    // 全部发送完
    if(sentBytes == fileBytes)
    {
        tSocket->close(); // 关socket
        QMessageBox::warning(this, "File Received",
            QString("%1 接受成功!").arg(fileName),
            QMessageBox::Ok);
        fileName.clear(); // 清空文件名
    }
}

/*--- 出错处理 ---*/
void ChatWindow::show_error(QAbstractSocket::SocketError)
{
    qDebug() << "*** Socket Error";
    fileName.clear();
}

/*--- 关聊天窗口事件 ---*/
void ChatWindow::closeEvent(QCloseEvent*)
{
    // 发出关窗口信号，通知好友名片
    emit close_chat();
}
