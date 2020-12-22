#include "../main.hpp"

//[host:port_network:password_network]
//127.0.0.1:5555:egrjgiure
static bool check_network(char const *av)
{
    size_t dot_host = 0;
    size_t dot_dot_str = 0;
    size_t len_str = strlen(av);

    //check dot dot
    for (size_t i = 0; i < len_str; i++)
    {
        if (av[i] == ':')
            dot_dot_str++;
        if (dot_dot_str < 2)
            if (!isdigit(av[i]) && av[i] != '.' && av[i] != ':')
                return (false);
    }
    if (dot_dot_str > 2)
        return (false);
    
    //check dot host
    for (size_t i = 0; av[i] != ':'; i++)
    {
        if (av[i] == '.')
            dot_host++;
    }
    if (dot_host != 3)
        return (false);
    return (true);
}

//<port> 2 22 222 2222
static bool check_port(char const *av)
{
    size_t len = strlen(av);
    
    if (len > 4)
        return (false);

    for (size_t i = 0; i < len; i++)
    {
        if (!isdigit(av[i]))
            return (false);
    }
    return (true);
}

bool check_av(int ac, char const **av)
{
    if (ac == 3)
    {
        if (!check_port(av[1]))
            return (false);
    }
    else if (ac == 4)
    {
        if (!check_network(av[1]) || !check_port(av[2]))
            return (false);
    }
    return (true);
}
