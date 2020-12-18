#include "main.hpp"

int main(int ac, char const **av)
{
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

    std::cout << "OK\n";

    return (0);
}