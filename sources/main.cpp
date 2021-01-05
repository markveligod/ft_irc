#include "./main.hpp"
#include "Class.IRC.hpp"

static IRC server;

void terminate_server(int signal)
{
	if (signal == SIGINT)
	{
		for (size_t i = 0; i < server.get_servers().size(); i++)
		{
			server.push_cmd_queue(server.get_servers()[i]->getSocketFd(), "SQUIT <servername> :terminate\r\n");
		}
	}
}

int main(int ac, char **av)
{
	std::vector<std::string> network;

	if (ac != 3 && ac != 4)
		utils::exit_error(ERR_COUNT, "ARG: ./ircserv [host:port_network:password_network] <port> <password>");
	if (!utils::check_av(ac, av))
		utils::exit_error(ERR_CHECKAV, "Incorrect enter to argv!");

	if (ac == 4)
		network = utils::split(av[1], ':');
	else
	{
		network.push_back(LOCALHOST);
		network.push_back(DEF_PORT);
		network.push_back(DEF_PASS);
	}

	//test cout 
	std::cout << network[0] << std::endl;
	std::cout << network[1] << std::endl;
	std::cout << network[2] << std::endl;
	std::cout << ((ac == 4) ? av[2] : av[1]) << std::endl;
	std::cout << ((ac == 4) ? av[3] : av[2]) << std::endl;

	IRC server(network[0], network[1], network[2], (ac == 4) ? av[2] : av[1], (ac == 4) ? av[3] : av[2]);
	IRC server_ssl(network[0], network[1], network[2], (ac == 4) ? av[2] : av[1]	, (ac == 4) ? av[3] : av[2]);
	//signal(SIGINT, terminate_server);
	if (ac == 4)
		server.create_socket_network();

	server.init_ssl();
	server.init_ctx();

	server.create_socket_local();
	while (true)
	// for (int i = 0; i < 3; i++)
	{
		server.init_fd_select();
		server.do_select();
		server.check_fd_select();
	}

	// SSL_CTX_free(_ctx);
	return (0);
}
