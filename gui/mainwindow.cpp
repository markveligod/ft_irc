#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->mess.push_back(QString("AAA"));
    this->mess.push_back(QString("BBB"));
    this->mess.push_back(QString("CCC"));
    this->mess.push_back(QString("DDD"));
    this->mess.push_back(QString("RRR"));
    this->mess.push_back(QString("TTT"));
    this->mess.push_back(QString("YYY"));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::RunTextBrowser()
{
    for (int i = 0; i < this->mess.size(); i++)
    {
        this->ui->textBrowser->append(this->mess[i]);
    }

}

void MainWindow::on_ButQuit_clicked()
{
    QMessageBox::StandardButton answer = QMessageBox::question(this, "Exit", "Are you sure?", QMessageBox::Yes | QMessageBox::No);
    if (answer == QMessageBox::Yes)
        QApplication::quit();
}
