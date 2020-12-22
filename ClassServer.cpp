#include "ClassServer.hpp"

Server::Server(const std::string &port_server): AServer(atoi(port_server.c_str()))
{
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
	Utils::print_line("Connected to the client");
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

void Server::recv_message()
{
    std::cout << "Client: ";
	recv(this->server, this->buffer, BUFFER_SIZE, 0);
	std::cout << this->buffer << std::endl;
	if (AServer::end_connection())
	{
		Utils::print_line("Client closed");
		this->server = 0;
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