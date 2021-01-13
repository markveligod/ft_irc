#include "./main.hpp"
#include "Class.IRC.hpp"

int g_exit;

void terminate_server(int signal)
{
	if (signal == SIGINT)
	{
		g_exit = 1;
	}
}

int main(int ac, char **av)
{
	std::vector<std::string> network;
	g_exit = 0;

	if (ac != 3 && ac != 4)
		utils::exit_error(ERR_COUNT, "ARG: ./ircserv [host:port_network:password_network] <port> <password>");
	// if (!utils::check_av(ac, av))
	// 	utils::exit_error(ERR_CHECKAV, "Incorrect enter to argv!");

	if (ac == 4)
	{
		network = utils::split(av[1], ':');
		if (network.size() < 3)
			network.push_back(string());
	}
	else
	{
		network.push_back(LOCALHOST);
		network.push_back(DEF_PORT);
		network.push_back(DEF_PASS);
	}

	IRC server(network[0], network[1], network[2], (ac == 4) ? av[2] : av[1], (ac == 4) ? av[3] : av[2]);
	IRC server_ssl(network[0], network[1], network[2], (ac == 4) ? av[2] : av[1]	, (ac == 4) ? av[3] : av[2]);
	signal(SIGINT, terminate_server);
	if (ac == 4)
		server.create_socket_network();

	server.init_ssl();
	server.init_ctx();

	server.create_socket_local();
	while (true)
	// for (int i = 0; i < 3; i++)
	{
		server.init_fd_select();
		// std::cout << "DEBUG: INIT DONE!\n";
		server.do_select();
		// std::cout << "DEBUG: DO_SELECT DONE!\n";
		server.check_fd_select();
		// std::cout << "DEBUG: CHECK_FD_SELECT DONE!\n";
		if (g_exit == 1 && server.get_servers().size() == 0 && server.isEmptyQuene() == true)
			exit(EXIT_SUCCESS);
		if (g_exit == 1 && server.get_servers().size() != 0)
		{
			std::cout << "DEBUG: CTRL + C DONE!\n";
			for (size_t i = 0; i < server.get_servers().size(); i++)
			{
				server.push_cmd_queue(server.get_servers()[i]->getSocketFd(), "SQUIT " + server.get_servers()[i]->getServerName() + " :terminate\r\n");
			}
			server.get_servers().clear();
		}
	}
	return (0);
}
