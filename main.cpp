#include "main.hpp"

int main(int ac, char const **av)
{
    std::vector<std::string> network;
    srand(time(NULL));
    
    if (ac != 3 && ac != 4)
        Utils::exit_error(ERR_COUNT, "ARG: ./ircserv [host:port_network:password_network] <port> <password>");
    if (!Utils::check_av(ac, av))
        Utils::exit_error(ERR_CHECKAV, "Incorrect enter to argv!");

    if (ac == 4)
        network = Utils::split(av[1], ':');
    else
    {
        network.push_back(DEF_HOST);
        network.push_back(Utils::get_random_port());
        network.push_back(DEF_PASS);
    }
 
    //test cout 
    std::cout << network[0] << std::endl;
    std::cout << network[1] << std::endl;
    std::cout << network[2] << std::endl;

    return (0);
}