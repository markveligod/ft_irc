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

    Server server = Server(network[0], atoi(network[1].c_str()), network[2], atoi((ac == 4) ? av[2] : av[1]), std::string((ac == 4) ? av[3] : av[2]));

    server.create_socket_locale();

	return (0);
}