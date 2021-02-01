#include <QApplication>
#include "../includes/main.hpp"
#include "mainwindow.h"

int main(int ac, char *av[])
{
	if (ac != 3 && ac != 4)
		utils::exit_error(ERR_COUNT, "ARG: ./ircserv [host:port_network:password_network] <port> <password>");
	if (!utils::check_av(ac, av))
		utils::exit_error(ERR_CHECKAV, "Incorrect enter to argv!");

    t_args* args = new t_args;
    args->_ac = ac;
    args->_av = av;

    pthread_t t;

    pthread_create(&t, NULL, start_irc, (void*)args);
    pthread_detach(t);

    QApplication a(ac, av);
    MainWindow w;
    w.RunTextBrowser();
    w.RunTextBrowser();
    w.RunTextBrowser();
    w.RunTextBrowser();
    w.show();

    return a.exec();
}
