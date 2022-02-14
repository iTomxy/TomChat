#include "itom.h"
#include "logindialog.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    /* 外部的变量 userName
     * 用于从LoginDialog接收用户名
     * 并传递给主界面
     * （这么做不太优雅，但还没想到更好的办法…）
     */
    QString userName;
    LoginDialog ld(&userName);
    if(ld.exec() == QDialog::Accepted)
    {
        iTom w(userName);
        w.show();
        return a.exec();
    }
    else
        return 0;
}
