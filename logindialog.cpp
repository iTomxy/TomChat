#include "logindialog.h"
#include "ui_logindialog.h"
#include <QMessageBox>
#include <QString>

LoginDialog::LoginDialog(QString *n, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LoginDialog),
    userame(n)
{
    ui->setupUi(this);
}

LoginDialog::~LoginDialog()
{
    delete ui;
}

void LoginDialog::on_loginBtn_clicked()
{
    if(ui->nameEdit->text() == "")
    {
        QMessageBox::warning(this, QString("Warning"),
            QString("User name cannot be empty!"),
            QMessageBox::Ok);
    }
    else
    {
        *userame = ui->nameEdit->text();
        accept();
    }
}
