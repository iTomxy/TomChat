#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QDialog>
class QString;

namespace Ui {
    class LoginDialog;
}

class LoginDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LoginDialog(QString *n, QWidget *parent = 0);
    ~LoginDialog();

private slots:
    void on_loginBtn_clicked();

private:
    Ui::LoginDialog *ui;
    QString *userame;
};

#endif // LOGINDIALOG_H
