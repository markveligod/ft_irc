#include "main.hpp"

int main(int ac, char const **av)
{
    std::vector<std::string> network;
    
    if (ac != 3 && ac != 4)
    {
        std::cerr << "\tARG: ./ircserv [host:port_network:password_network] <port> <password>\n";
        return (ERR_COUNT);
    }

    if (!check_av(ac, av))
    {
        std::cerr << "\tIncorrect enter to arg\n";
        return (ERR_CHECKAV);
    }

    if (ac == 4)
        network = split(av[1], ':');
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
     

    return (0);
}