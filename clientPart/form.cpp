#include "form.h"
#include "ui_form.h"
#include <QMessageBox>

Form::Form(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Form)
{
    ui->setupUi(this);
}

Form::~Form()
{
    delete ui;
}

void Form::on_pushButton_clicked()
{
    this->pass = ui->linepass->text();
    this->port = ui->lineport->text();
    this->nick = ui->linenick->text();
    this->user = ui->lineuser->text();
    if (pass.isEmpty() || port.isEmpty() || nick.isEmpty() || user.isEmpty())
        QMessageBox::warning(this, "Error", "Заполни все поля не будь бякой!");
    else
    {
        this->isCheck = true;
        this->close();
    }
}

bool Form::isChecked() const {return (this->isCheck);}

void Form::getParam(QString &pass, QString &port, QString & nick, QString & user)
{
    pass = this->pass;
    port = this->port;
    nick = this->nick;
    user = this->user;
}
