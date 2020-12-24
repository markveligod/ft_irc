#include "Class.Server.hpp"

Server::Server(const std::string &port_server, const std::string &pass): AServer(atoi(port_server.c_str()))
{
	this->pass = pass;
	for (int i = 0; i < 1024; i++)
		this->socket_type[i] = FD_FREE;
	// цикл не нужен
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
	// без изменений
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
	Utils::print_line("Check password...");
	recv(this->server, this->buffer, BUFFER_SIZE, 0);
	if ((std::string)this->buffer == this->pass)
	{
		strcpy(this->buffer, "Server connected!\n");	
		send(this->server, this->buffer, BUFFER_SIZE, 0);
		Utils::print_line("Connected to the client#" + Utils::convert_int_to_str(this->server));
	}
	else
	{
		strcpy(this->buffer, "FAIL");	
		send(this->server, this->buffer, BUFFER_SIZE, 0);
		Utils::print_line("Incorrect password from client#" + Utils::convert_int_to_str(this->server));
	}
}

/*
**==========================
** send_message - отправляет буфер
**==========================
*/

void Server::send_message()
{

}

/*
**==========================
** recv_message - принимает буфер
**==========================
*/

void Server::recv_message(int i)
{
    std::cout << "Client#" << i << ": ";
	// recv(i, this->buffer, BUFFER_SIZE, 0);
	if (recv(i, this->buffer, BUFFER_SIZE, 0) == 0)
		strcpy(AServer::buffer, "#");
	else
		std::cout << this->buffer << std::endl;
	if (AServer::end_connection())
	{
		std::cout << CYAN << "[SERVER]: " << YELLOW << "Client#" << i << " closed" << std::endl << RESET;
		this->server = 0;
		this->socket_type[i] = FD_FREE;
		// socket_type.erase(i);
	}
}

/*
**==========================
** init_fd_select - добавление сокета в сет файловых дескрипотов для селекта
**==========================
*/

void Server::init_fd_select()
{
	FD_ZERO(&this->client_sockets);
	for (int i = 0; i < 1024; i++)
	// for (std::map<int, int>::iterator it = socket_type.begin(); it != socket_type.end(); it++)
	{
		if (this->socket_type[i] != FD_FREE)
		// if (it->second != FD_FREE)
			FD_SET(i, &this->client_sockets);
			// FD_SET(it->first, &this->client_sockets);
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
	for (int i = 0; i < 1024 && this->select_res > 0; i++)
	// for (std::map<int, int>::iterator it = socket_type.begin(); it != socket_type.end(); it++)
	{
		if (FD_ISSET(i, &this->client_sockets))
		// if (FD_ISSET(it->first, &this->client_sockets))
		{
			if (this->socket_type[i] == FD_CLIENT)
			// if (it->second == FD_CLIENT)
			{
				this->recv_message(i);
				// this->recv_message(it->first);
				client_part->set_buffer(this->get_buffer().c_str());
				client_part->send_message();
			}
			if (this->socket_type[i] == FD_SERVER)
			// if (it->second == FD_SERVER)
			{
				this->socket_accept();
				this->socket_type[this->server] = FD_CLIENT;
				// без изменений
			}
			this->select_res--;
		}
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
