#include "utils.hpp"

/*
**==========================
** check_av - проверяет аргументы
** check_network - проверяет сеть другого сервера
** check_port - проверяет порт текущего сервера
**==========================
*/

//[host:port_network:password_network]
//127.0.0.1:5555:egrjgiure
namespace utils
{
	static bool
	check_network(char const* av)
	{
		std::string network(av);

		size_t colons = std::count(network.begin(), network.end(), ':');
		if (colons != 2u)
			return false;
		
		size_t first_colon = network.find(':');														// ищем индекс первого ':'
		std::string network_address = network.substr(0, first_colon);								// получаем адрес сети
		if(!gethostbyname(network_address.c_str()))													// проверяем, что адрес корректный
			return false;
		
		size_t second_colon = network.find(':', first_colon + 1);									// ищем индекс второго ':'
		// if (second_colon == first_colon + 1 || second_colon == network.size() - 1)				// проверяем, что порт и пароль не пустые
		// 	return false;

		std::string network_port = network.substr(first_colon + 1, second_colon - first_colon - 1);	// получаем порт сети
		int port = atoi(network_port.c_str());														// проверяем, что порт не больше макс short

		if (port > static_cast<int>(std::numeric_limits<short>::max()))
			return false;

		return (true);
	}

	//<port> 2 22 222 2222
	static bool
	check_port(char const* av)
	{
		size_t len = strlen(av);
		
		if (len > 4)
			return (false);

		for (size_t i = 0; i < len; i++)
		{
			if (!isdigit(av[i]))
				return (false);
		}

		int max_port = static_cast<int>(std::numeric_limits<short>::max());
		if (atoi(av) > max_port)
			return false;

		return (true);
	}

	bool
	check_av(int ac, char **av)
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

	/*
	**==========================
	** print_line - печатает статус сервера
	** print_error - печатает ошибку
	** exit_error - вылетает с ошибкой
	**==========================
	*/

	void
	print_line(std::string msg)
	{
		std::cout << CYAN << "[SERVER]: " << YELLOW << msg << std::endl << RESET;
	}

	void
	print_client(int n, std::string msg)
	{
		std::cout << PURPLE << "\n[CLIENT " << n << "]: " << YELLOW << msg << std::endl << RESET;
	}

	void
	print_message(int fd, std::vector<std::string> &buffer_cmd)
	{
		std::stringstream ss;
		ss << "Recieved commands(" << buffer_cmd.size() << "):";
		utils::print_client(fd, ss.str());
		for (size_t i = 0; i < buffer_cmd.size(); i++)
			std::cout << "\t " << PURPLE << std::setw(2) << i << ": " << YELLOW << buffer_cmd[i] << std::endl << RESET;
	}

	void
	print_command(int fd, std::string msg)
	{
		std::cout << BLUE << "\n[COMM " << fd << "]: " << YELLOW << msg << std::endl << RESET;
	}

	void
	print_error(int num_error, std::string fail_msg)
	{
		std::cerr << CYAN << "[SERVER]: " << RED << "ERROR: " << RESET << "code: " << num_error << " " << YELLOW << fail_msg << std::endl << RESET;
	}

	void exit_error(int num_error, std::string fail_msg)
	{
		print_error(num_error, fail_msg);
		exit(num_error);
	}

	/*
	**==========================
	** split - возвращает вектор <std::string> распарсенных данных по char type
	**==========================
	*/

	std::vector<std::string>
	split(std::string str, char type)
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
	** int_to_str - конвертирует int в std::string
	**==========================
	*/

	std::string
	int_to_str(int num)
	{
		std::stringstream ss;
		ss << num;
		return (ss.str());
	}

	
} //namespace