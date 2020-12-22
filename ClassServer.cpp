#include "ClassServer.hpp"

Server::Server(const std::string &port_server): AServer(atoi(port_server.c_str()))
{
	int i;

	i = 0;
	while (i < 1024)
		this->socket_type[i++] = FD_FREE;
	Utils::print_line("Constructor server done!");
}

/*
**==========================
** create_socket - 
**==========================
*/

void Server::create_socket()
{
    if ((this->fd_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		Utils::exit_error(ERR_SOCKET, "Esteblishing socket error");

	bzero(&this->server_addr, sizeof(this->server_addr));
	this->server_addr.sin_family 		= AF_INET; //IP(v4)
	this->server_addr.sin_addr.s_addr 	= htons(INADDR_ANY);
	this->server_addr.sin_port			= htons(this->port);

    Utils::print_line("Socket created!");
}

/*
**==========================
** connection - соединяет сокет с портом
**==========================
*/

void Server::connection()
{
    this->size = sizeof(this->server_addr);

	if (bind(this->fd_socket, reinterpret_cast<struct sockaddr *>(&this->server_addr), sizeof(this->server_addr)) < 0)
		Utils::exit_error(ERR_BIND, "Binding error");
    Utils::print_line("Socket binded!");
}

/*
**==========================
** socket_listen - при вызове слушает сокет
**==========================
*/

void Server::socket_listen()
{
	if (listen(this->fd_socket, 10) < 0)
		Utils::exit_error(ERR_LISTEN, "Listening error");
    Utils::print_line("Listening...");

	this->socket_type[this->fd_socket] = FD_SERVER;
}

/*
**==========================
** socket_accept - при вызове соединяет сокет с клиентом
**==========================
*/

void Server::socket_accept()
{
	if ((this->server = accept(this->fd_socket, reinterpret_cast<struct sockaddr *>(&this->server_addr), &(this->size))) < 0)
		Utils::exit_error(ERR_ACCEPT, "Accepting error");
    Utils::print_line("Connection accepted!");
	strcpy(this->buffer, "Server connected!\n");
	send(this->server, this->buffer, BUFFER_SIZE, 0);
	std::cout << CYAN << "[SERVER]: " << YELLOW << "Connected to the client#" << this->server << std::endl << RESET;
}

/*
**==========================
** send_message - отправляет буфер
**==========================
*/

void Server::send_message()
{
	char buffer[BUFFER_SIZE];
	Utils::print_line(" ");
	std::cin.getline(buffer, BUFFER_SIZE);
	send(this->server, this->buffer, BUFFER_SIZE, 0);
}

/*
**==========================
** recv_message - принимает буфер
**==========================
*/

void Server::recv_message(int i)
{
    std::cout << "Client#" << i << ": ";
	recv(i, this->buffer, BUFFER_SIZE, 0);
	std::cout << this->buffer << std::endl;
	if (AServer::end_connection())
	{
		std::cout << CYAN << "[SERVER]: " << YELLOW << "Client#" << i << " closed" << std::endl << RESET;
		this->server = 0;
	}
}

/*
**==========================
** init_fd_select - добавление сокета в сет файловых дескрипотов для селекта
**==========================
*/

void Server::init_fd_select()
{
	int i;

	i = 0;
	FD_ZERO(&this->client_sockets);
	while (i < 1024)
	{
		if (this->socket_type[i] != FD_FREE)
			FD_SET(i, &this->client_sockets);
		i++;
	}
}

/*
**==========================
** do_select - селект!
**==========================
*/

void Server::do_select()
{
	if ((this->select_res = select(1024, &this->client_sockets, NULL, NULL, NULL)) < 0)
		Utils::print_error(16, "SELECT");
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

void Server::check_fd_select(Client *client_part)
{
	int i;

	i = 0;
	while ((i < 1024) && (this->select_res > 0))
	{
		if (FD_ISSET(i, &this->client_sockets))
		{
			if (this->socket_type[i] == FD_CLIENT)
			{
				this->recv_message(i);
				client_part->set_buffer(this->get_buffer().c_str());
				client_part->send_message();
			}
			if (this->socket_type[i] == FD_SERVER)
			{
				std::cout << i << "accept" << std::endl;
				this->socket_accept();
				this->socket_type[this->server] = FD_CLIENT;
			}
			this->select_res--;
		}
		i++;
	}
}

bool Server::check_fd_server()
{
	return ((this->server > 0));
}

/*
**==========================
** get_buffer - возвращает полученный из recv буфер
**==========================
*/

std::string const Server::get_buffer()
{
    return (std::string(this->buffer));
}
