#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMessageBox>
#include <QString>
#include <QVector>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void RunTextBrowser();

private slots:
    void on_ButQuit_clicked();

private:
    Ui::MainWindow *ui;
    QVector<QString> mess;
};

#endif // MAINWINDOW_H
