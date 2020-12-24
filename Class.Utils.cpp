#include "Class.Utils.hpp"

/*
**==========================
** check_av - проверяет аргументы
** check_network - проверяет сеть другого сервера
** check_port - проверяет порт текущего сервера
**==========================
*/

//[host:port_network:password_network]
//127.0.0.1:5555:egrjgiure
bool Utils::check_network(char const *av)
{
    size_t dot_host = 0;
    size_t dot_dot_str = 0;
    size_t len_str = strlen(av);

    //check colon
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
bool Utils::check_port(char const *av)
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

bool Utils::check_av(int ac, char const **av)
{
    if (ac == 3)
    {
        if (!Utils::check_port(av[1]))
            return (false);
    }
    else if (ac == 4)
    {
        if (!Utils::check_network(av[1]) || !Utils::check_port(av[2]))
            return (false);
    }
    return (true);
}


/*
**==========================
** get_random_port - возвращает рандомную строку порта от 1000 до 6999
**==========================
*/

std::string Utils::get_random_port()
{
    std::string temp;
    const char *local = "123456789";
    const char *first_local = "123456";

    temp.push_back(first_local[rand() % 6]);
    for (size_t i = 0; i < 3; i++)
    {
        temp.push_back(local[rand() % 9]);
    }
    return (temp);
}

/*
**==========================
** print_line - печатает статус сервера
** print_error - печатает ошибку
** exit_error - вылетает с ошибкой
**==========================
*/

void Utils::print_line(std::string msg)
{
    std::cout << CYAN << "[SERVER]: " << YELLOW << msg << std::endl << RESET;
}

void Utils::print_error(int num_error, std::string fail_msg)
{
    std::cerr << CYAN << "[SERVER]: " << RED << "ERROR: " << RESET << "code: " << num_error << " " << YELLOW << fail_msg << std::endl << RESET;
}

void Utils::exit_error(int num_error, std::string fail_msg)
{
    print_error(num_error, fail_msg);
    exit(num_error);
}

/*
**==========================
** split - возвращает вектор <std::string> распарсенных данных по char type
**==========================
*/

std::vector<std::string> Utils::split(std::string str, char type)
{
    std::vector<std::string> temp_vec;
    std::stringstream ss(str);
    std::string temp_str;
    while(std::getline(ss, temp_str, type))
    {
        temp_vec.push_back(temp_str);
    }
    return (temp_vec);
}

/*
**==========================
** convert_int_to_str - конвертирует int в std::string
**==========================
*/

std::string Utils::convert_int_to_str(int num)
{
    std::stringstream ss;
    ss << num;
    return (ss.str());
}
