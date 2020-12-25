#include "./main.hpp"
#include "./Class.Server.hpp"

int main(int ac, char **av)
{
	std::vector<std::string> network;

    if (ac != 3 && ac != 4)
        Utils::exit_error(ERR_COUNT, "ARG: ./ircserv [host:port_network:password_network] <port> <password>");
    if (!Utils::check_av(ac, av))
        Utils::exit_error(ERR_CHECKAV, "Incorrect enter to argv!");

    if (ac == 4)
        network = Utils::split(av[1], ':');
    else
    {
        network.push_back(DEF_HOST);
        network.push_back(DEF_PORT);
        network.push_back(DEF_PASS);
    }

    //test cout 
    std::cout << network[0] << std::endl;
    std::cout << network[1] << std::endl;
    std::cout << network[2] << std::endl;
    std::cout << ((ac == 4) ? av[2] : av[1]) << std::endl;
    std::cout << ((ac == 4) ? av[3] : av[2]) << std::endl;

    Server server(network[0], network[1], network[2], (ac == 4) ? av[2] : av[1], (ac == 4) ? av[3] : av[2]);

    if (ac == 4)
    {
        server.create_socket_network();
        server.create_socket_locale();
        while (true)
        {

        }
    }
    else if (ac == 3)
    {
        server.create_socket_locale();
        while (true)
        {
            server.init_fd_select();
			server.do_select();
			server.check_fd_select();
        }
    }

	return (0);
}