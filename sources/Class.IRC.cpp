#include "Class.IRC.hpp"

# define FD_FREE 0
# define FD_CLIENT 1
# define FD_SERVER 2

IRC::IRC() {}

IRC::IRC(std::string network_ip,
				std::string network_port,
				std::string network_pass,
				std::string localhost_port,
				std::string localhost_pass)
{
    _network_ip = network_ip;
    _network_port = std::atoi(network_port.c_str());
    _network_pass = network_pass;
    _localhost_pass = localhost_pass;
	_localhost = Socket(LOCALHOST, std::atoi(localhost_port.c_str()));
    Utils::print_line("Constructor IRC done!");
	Utils::print_line("Socket local done!");
}

void IRC::create_socket_local()
{
	// local socket created on IRC conctructor

	_array_fd_select[_localhost._socket()] = FD_SERVER;
    Utils::print_line("Socket local FD done!");

    _localhost._bind();
    Utils::print_line("Socket local bind done!");

    _localhost._listen();
    Utils::print_line("Socket local listen...");
}

void IRC::create_socket_network()
{
	_network = Socket(_network_ip.c_str(), _network_port);
	Utils::print_line("Socket network done!");
    _network._socket();
    Utils::print_line("Socket network FD done!");
    _network._connect();
    Utils::print_line("Socket network connection!");
}

/*
**==========================
** init_fd_select - добавление сокета в сет файловых дескрипотов для селекта
**==========================
*/

void IRC::init_fd_select()
{
    FD_ZERO(&_fd_set_sockets);
	for (std::map<int, int>::iterator it = _array_fd_select.begin(); it != _array_fd_select.end(); it++)
    {
		FD_SET(it->first, &_fd_set_sockets);
    }
}

/*
**==========================
** do_select - селект!
**==========================
*/

void IRC::do_select()
{
	if ((_select_res = select(FD_SETSIZE, &_fd_set_sockets, NULL, NULL, NULL)) < 0)
		Utils::print_error(ERR_SELECT, "SELECT");
}

typedef void (Message::*doCommand)(void *);

void IRC::do_command(Message * message)
{
	std::string cmd_name[2] = {"NICK",
							   "PASS"};
	doCommand 	cmd_func[2]	= {&Message::cmd_nick,
							   &Message::cmd_pass};
	void *		cmd_var[2]	= {(void *)&this->_clients,
						       (void *)&this->_clients};

	for (int i = 0; i < 2; i++)
		if (cmd_name[i] == message->getCommand())
		{
			(message->*(cmd_func[i]))(cmd_var[i]);
			return ;
		}
	Utils::print_error(123, "Command not found");
}

/*
**==========================
** check_fd_select - проходимся по всем дескрипторам, если встречаем тот, который
** 					 содержится в отслеживаемых селектом (т.е. он содержится в сете);
**					 - Если это дескриптор сервера, то нужно подтвердить соединение и
**		  			 установить тип этого дескриптора, как клиентский;
**					 - Если это дескриптор клиента - то нужно получить от него сообщение и 
**					 отослать серверу;
**==========================
*/

void IRC::check_fd_select()
{
	for (std::map<int, int>::iterator it = _array_fd_select.begin(); it != _array_fd_select.end() && _select_res > 0; it++)
	{
		if (FD_ISSET(it->first, &_fd_set_sockets))
		{
			if (it->second == FD_CLIENT)
			{
				char buffer[512 + 1];
				int n = 0;
				std::cout << "Client#" << it->first << ": ";
				if (((n = recv(it->first, buffer, 512, 0))) == 0)
				{
					std::cout << "connection closed\n";
					_array_fd_select.erase(it->first);
				}
				else
				{
					Message mess;

					buffer[n] = '\0';
					std::cout << buffer << std::endl;
					mess.pars_str(buffer);
					this->do_command(&mess);
					if (mess.getCommand() == "NICK")
						std::cout << this->_clients[0]->getNickname() << std::endl;
					_network._send(buffer);
				}
			}
			//Раздел для приемки сообщений из селекта
			if (it->second == FD_SERVER)
			{
				//Перед тем как добавлять user проверяем пароль и заполняем структуру
				//Message mess;
				//User	*user;

				//mess.pars_str("PASS 123");
				//this->do_command(&mess);
				//mess.pars_str("NICK mark");
				//this->do_command(&mess);
				//std::cout << this->_users[0]->getNickname() << std::endl;

				// _accept() возвращает fd клиента, который мы добавляем в map
				// в качестве ключа, в качестве значения добавляем FD_CLIENT
				int client = _localhost._accept();
				_array_fd_select[client] = FD_CLIENT;
				char response[] = "Accepted\n";
				std::cout << "New client#" << client << " joined server\n";
				Client *newclient = new Client(client);
				this->_clients.push_back(newclient);
				send(client, reinterpret_cast<void *>(response), 10, 0);
			}
			_select_res--;
		}
	}
}