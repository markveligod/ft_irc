#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QString>
#include <QTcpSocket>
#include <QDataStream>
#include <QTimer>
class ClientStuff;
namespace Ui { class MainWindow;}
#include "clientStuff.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    
public slots:
    void setStatus(bool newStatus);
    void receivedSomething(QString msg);
    void gotError(QAbstractSocket::SocketError err);


private slots:
    void on_pushButton_send_clicked();
    void on_pushButton_connect_clicked();
    void on_pushButton_disconnect_clicked();

    void readyRead();

private:
    Ui::MainWindow *ui;
    ClientStuff *client;
    QString mynick;
};

#endif // MAINWINDOW_H
