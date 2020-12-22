#include "main.hpp"

int main(int ac, char const **av)
{
    std::vector<std::string> network;
    srand(time(NULL));
    
    if (ac != 3 && ac != 4)
        exit_error(ERR_COUNT, "ARG: ./ircserv [host:port_network:password_network] <port> <password>");
    if (!check_av(ac, av))
        exit_error(ERR_CHECKAV, "Incorrect enter to argv!");

    if (ac == 4)
        network = split(av[1], ':');
    else
    {
        network.push_back(DEF_HOST);
        network.push_back(get_random_port());
        network.push_back(DEF_PASS);
    }
 
    //test cout 
    std::cout << network[0] << std::endl;
    std::cout << network[1] << std::endl;
    std::cout << network[2] << std::endl;
    
    Server server;

    return (0);
}