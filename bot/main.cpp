// #include <map>
// #include <netinet/in.h>
// #include <sys/socket.h>
#include <fstream>
#include "main.hpp"
#include "Class.Bot.hpp"
#include "Class.Socket.hpp"

#define BOT_NAME "pogoda"

int g_exit = 0;

void terminate(int signal)
{
	if (signal == SIGINT)
		g_exit = 1;	
}

int main(int ac, char** av)
{
    if (ac != 2)
		utils::exit_error(ERR_COUNT, "ARG: ./pogoda host:port_network:password_network");
	if (!utils::check_network(av[1]))
		utils::exit_error(ERR_CHECKAV, "Incorrect enter to argv!");

    vector<string> network;
	network = utils::split(av[1], ':');

	Socket sock(network[0].c_str(), atoi(network[1].c_str()));
	sock._socket();
	if (sock._connect() < 0)
		exit(1);

	Bot bot;
	bot._array_fd_select[sock._fd] = FD_CLIENT;

	string register_message = "PASS " + network[2] + "\r\n" +
							  "NICK " + BOT_NAME + "\r\n" +
							  "USER " + BOT_NAME + " 1 1 :" + BOT_NAME + " bot\r\n";
	bot.push_cmd_queue(sock._fd, register_message);
	
	signal(SIGINT, terminate);

	while (true)
	{
		bot.init_fd_select();
		bot.do_select();
		bot.check_fd_select();
		if (g_exit)
		{
			send(sock._fd, "QUIT :\r\n", 10, 0);
			exit(0);
		}
	}
}