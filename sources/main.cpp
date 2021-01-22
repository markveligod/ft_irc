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
	g_exit = 0;

	if (ac != 3 && ac != 4)
		utils::exit_error(ERR_COUNT, "ARG: ./ircserv [host:port_network:password_network] <port> <password>");
	if (!utils::check_av(ac, av))
		utils::exit_error(ERR_CHECKAV, "Incorrect enter to argv!");

	vector<string> network;
	if (ac == 4)
	{
		network = utils::split(av[1], ':');
		network.push_back(string(av[2]));
		network.push_back(string(av[3]));
	}
	else
	{
		network.insert(network.begin(), 3, string());
		network.push_back(string(av[1]));
		network.push_back(string(av[2]));
	}
	
	IRC server(network[0], network[1], network[2], network[3], network[4]);

	signal(SIGINT, terminate_server);
	if (ac == 4)
		server.create_socket_network();

	server.init_ssl();
	server.init_ctx();

	server.create_socket_local();
	while (true)
	{
		server.init_fd_select();
		// std::cout << "DEBUG: INIT DONE!\n";
		server.do_select();
		// std::cout << "DEBUG: DO_SELECT DONE!\n";
		server.check_fd_select();
		// std::cout << "DEBUG: CHECK_FD_SELECT DONE!\n";
		if (g_exit == 1 && server.get_servers().size() == 0 && server.is_empty_queue() == true)
			exit(EXIT_SUCCESS);
		// std::cout << "DEBUG: G_EXIT: " << g_exit << " SERVER SIZE: " << server.get_servers().size() << " IS_EMPTY: " << server.is_empty_queue() << std::endl;
		if (g_exit == 1 && server.get_servers().size() != 0)
		{
			std::cout << "DEBUG: CTRL + C DONE!\n";
			for (size_t i = 0; i < server.get_servers().size(); i++)
			{
				server.push_cmd_queue(server.get_servers()[i]->getSocketFd(), "SQUIT " + server.get_servers()[i]->getName() + " :terminate\r\n");
			}
			server.get_servers().clear();
		}
	}
	return 0;
}

