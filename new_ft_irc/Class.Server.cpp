#include "./Class.Server.hpp"

# define FD_FREE 0
# define FD_CLIENT 1
# define FD_SERVER 2

Server::Server() {}

Server::Server(std::string ip_network, std::string port_network,  std::string pass_network, std::string port_curr, std::string pass_curr)
{
    this->ip_network = ip_network;
    this->port_network = std::atoi(port_network.c_str());
    this->pass_network = pass_network;
    this->port_curr = std::atoi(port_curr.c_str());
    this->pass_curr = pass_curr;
    Utils::print_line("Constructor Server done!");
}

void Server::create_socket_locale()
{
    this->server_locale = Socket(this->ip_network.c_str(), this->port_curr);
    Utils::print_line("Socket locale done!");

	array_fd_select[this->server_locale._socket()] = FD_SERVER;
    // int first_fd = this->server_locale._socket();
    // this->array_fd_select.push_back(first_fd);
    Utils::print_line("Socket locale FD done!");

    this->server_locale._bind();
    Utils::print_line("Socket locale bind done!");

    this->server_locale._listen();
    Utils::print_line("Socket locale listen...");
}

void Server::create_socket_network()
{
    this->server_network = Socket(this->ip_network.c_str(), this->port_network);
    Utils::print_line("Socket network done!");
    this->server_network._socket();
    Utils::print_line("Socket network FD done!");
    this->server_network._connect();
    Utils::print_line("Socket network connection!");
}

/*
**==========================
** init_fd_select - добавление сокета в сет файловых дескрипотов для селекта
**==========================
*/

void Server::init_fd_select()
{
    FD_ZERO(&this->fd_set_sockets);
	for (std::map<int, int>::iterator it = array_fd_select.begin(); it != array_fd_select.end(); it++)
    {
		FD_SET(it->first, &this->fd_set_sockets);
    }
}

/*
**==========================
** do_select - селект!
**==========================
*/

void Server::do_select()
{
	if ((this->select_res = select(FD_SETSIZE, &fd_set_sockets, NULL, NULL, NULL)) < 0)
		Utils::print_error(ERR_SELECT, "SELECT");
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

void Server::check_fd_select()
{
	for (std::map<int, int>::iterator it = array_fd_select.begin(); it != array_fd_select.end() && this->select_res > 0; it++)
	{
		if (FD_ISSET(it->first, &this->fd_set_sockets))
		{
			if (it->second == FD_CLIENT)
			{
				char buffer[512 + 1];
				int n = 0;
				std::cout << "Client#" << it->first << ": ";
				if (((n = recv(it->first, buffer, 512, 0))) == 0)
				{
					std::cout << "connection closed\n";
					array_fd_select.erase(it->first);
				}
				else {
					buffer[n] = '\0';
					std::cout << buffer << std::endl;
				}
			}
			//Раздел для приемки сообщений из селекта
			if (it->second == FD_SERVER)
			{
				// _accept() возвращает fd клиента, который мы добавляем в map
				// в качестве ключа, в качестве значения добавляем FD_CLIENT
				int client = server_locale._accept();
				this->array_fd_select[client] = FD_CLIENT;
				char response[] = "Accepted\n";
				send(client, reinterpret_cast<void *>(response), 10, 0);
			}
			this->select_res--;
		}
	}
}