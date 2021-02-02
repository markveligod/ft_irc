#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <iostream>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->pushButton_disconnect->setVisible(false);

    client = new ClientStuff("localhost", 1080);

    setStatus(client->getStatus());

    connect(client, &ClientStuff::hasReadSome, this, &MainWindow::receivedSomething);
    connect(client, &ClientStuff::statusChanged, this, &MainWindow::setStatus);
    // FIXME change this connection to the new syntax
    connect(client->tcpSocket, SIGNAL(error(QAbstractSocket::SocketError)),
            this, SLOT(gotError(QAbstractSocket::SocketError)));
}

MainWindow::~MainWindow()
{
    delete client;
    delete ui;
}

void MainWindow::setStatus(bool newStatus)
{
    if(newStatus)
    {
        ui->label_status->setText(
                    tr("<font color=\"green\">CONNECTED</font>"));
        ui->pushButton_connect->setVisible(false);
        ui->pushButton_disconnect->setVisible(true);
    }
    else
    {
        ui->label_status->setText(
                tr("<font color=\"red\">DISCONNECTED</font>"));
        ui->pushButton_connect->setVisible(true);
        ui->pushButton_disconnect->setVisible(false);
    }
}

void MainWindow::receivedSomething(QString msg)
{
    ui->textBrowser_log->append(msg);
}

void MainWindow::gotError(QAbstractSocket::SocketError err)
{
    //qDebug() << "got error";
    QString strError = "unknown";
    switch (err)
    {
        case 0:
            strError = "Connection was refused";
            break;
        case 1:
            strError = "Remote host closed the connection";
            break;
        case 2:
            strError = "Host address was not found";
            break;
        case 5:
            strError = "Connection timed out";
            break;
        default:
            strError = "Unknown error";
    }

    ui->textBrowser_log->append(strError);
}

void MainWindow::on_pushButton_connect_clicked()
{
    client->connect2host();
    connect(client->tcpSocket, &QTcpSocket::readyRead, this, &MainWindow::readyRead);
    client->tcpSocket->write((QString(tr("PASS 123\r\nNICK mark\r\nUSER client client client client\r\n"))).toLatin1());
}

void MainWindow::on_pushButton_send_clicked()
{
    client->tcpSocket->write(QString("%1 %2").arg(ui->lineEdit_message->text()).arg(QString(tr("\r\n"))).toLatin1());
    this->receivedSomething(QString("%1 %2").arg(QString(tr("SEND: "))).arg(ui->lineEdit_message->text()));
    ui->lineEdit_message->clear();
}

void MainWindow::on_pushButton_disconnect_clicked()
{
    this->receivedSomething(QString("SEND: QUIT :я ухожу от вас"));
    client->tcpSocket->write((QString(tr("QUIT :я ухожу от вас\r\n"))).toLatin1());
    client->closeConnection();
}

void MainWindow::readyRead()
{
    QByteArray totol_data, data_buffer;
    while(1)
    {
        data_buffer = client->tcpSocket->read(1024);
        if(data_buffer.isEmpty())
            break;
        totol_data.append(data_buffer);
    }
    QString str(totol_data);
    this->receivedSomething(str);
}
