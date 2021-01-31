#include <QApplication>
#include <QWidget>
 
int main(int argc, char *argv[]) {
 
    // Каждое Qt5-приложение (за исключением консольных) должно включать следующую строку
    QApplication app(argc, argv);
 
    // Главный виджет, который представляет окно нашей программы
    QWidget window;
 
    window.resize(250, 150); // изменяем размер виджета
    window.setWindowTitle("Simple example"); // устанавливаем заголовок для главного окна 
    window.show(); // выводим виджет на экране
 
    // С помощью метода exec() запускаем основной цикл нашей программы
    return app.exec();
}
