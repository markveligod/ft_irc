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
	bool
	check_network(char const* av)
	{
		std::string network(av);
		std::vector<std::string> net = utils::split(av, ':');

		if (net.size() != 3)
			return false;
		
		if(!gethostbyname(net[0].c_str()))								// проверяем, что адрес сети корректный
			return false;

		if (net[1].empty() || net[2].empty())							// проверяем, что порт и пароль не пустые
			return false;

		if (!check_port(net[1].c_str()))								// проверяем порт
			return false;

		return true;
	}

	//<port> 2 22 222 2222
	bool
	check_port(char const* av)
	{
		size_t len = strlen(av);
		
		if (len > 4)
			return false;

		for (size_t i = 0; i < len; i++)
		{
			if (!isdigit(av[i]))
				return false;
		}

		int max_port = static_cast<int>(std::numeric_limits<short>::max());
		if (atoi(av) > max_port)
			return false;

		return true;
	}

	bool
	check_av(int ac, char **av)
	{
		if (ac == 3)
			return (check_port(av[1]) && (av[2][0] != '\0'));		// av[1] - port, av[2] - password
		else if (ac == 4)
			return (check_network(av[1]) && check_port(av[2]) && (av[3][0] != '\0'));
		else
			return false;
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

	/*
	**==========================
	** is_equal - сравнивает без учета регистра
	**==========================
	*/

	bool
	is_equal(std::string s1, std::string s2)
	{
		utils::less comp;
		return !(comp(s1, s2)) && !comp(s2, s1);
	}

	unsigned long
	get_time()
	{
		time_t seconds = time(NULL);
		std::stringstream ss;

		ss << seconds;
		return (atoi(ss.str().c_str()));
	}

} //namespace