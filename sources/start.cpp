#include "main.hpp"
#include "Class.IRC.hpp"

int g_exit = 0;

void terminate_server(int signal)
{
	if (signal == SIGINT)
	{
		g_exit = 1;
	}
}

void* start_irc(void* args)
{
	vector<string> network;
	if (((t_args*)args)->_ac == 4)
	{
		network = utils::split(((t_args*)args)->_av[1], ':');
		network.push_back(string(((t_args*)args)->_av[2]));
		network.push_back(string(((t_args*)args)->_av[3]));
	}
	else
	{
		network.insert(network.begin(), 3, string());
		network.push_back(string(((t_args*)args)->_av[1]));
		network.push_back(string(((t_args*)args)->_av[2]));
	}
	
	IRC server(network[0], network[1], network[2], network[3], network[4]);

	signal(SIGINT, terminate_server);
	if (((t_args*)args)->_ac == 4)
		server.create_socket_network();

	server.init_ssl();
	server.init_ctx();

	server.create_socket_local();
	while (true)
	{
		server.init_fd_select();
		server.do_select();
		server.check_fd_select();
		if (g_exit == 1 && server.get_servers().size() == 0 && server.is_empty_queue() == true)
			exit(EXIT_SUCCESS);
		if (g_exit == 1 && server.get_servers().size() != 0)
		{
			for (size_t i = 0; i < server.get_servers().size(); i++)
			{
				int fd = server.get_servers()[i]->getSocketFd();
				if (server.get_servers()[i]->getHopcount() == 1)
					server.push_cmd_queue(fd, "SQUIT " + server.get_server_name() + " :terminate\r\n");
			}
			server.get_servers().clear();
		}
	}
	return 0;
}
