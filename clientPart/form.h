#ifndef FORM_H
#define FORM_H

#include <QDialog>

namespace Ui {
class Form;
}

class Form : public QDialog
{
    Q_OBJECT

public:
    explicit Form(QWidget *parent = nullptr);
    ~Form();
    bool isChecked() const;
    void getParam(QString &pass, QString &port, QString & nick, QString & user);

private slots:
    void on_pushButton_clicked();

private:
    Ui::Form *ui;
    QString nick;
    QString user;
    QString pass;
    QString port;
    bool isCheck = false;
};

#endif // FORM_H
