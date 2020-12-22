#include "ClassClient.hpp"

/*
**==========================
** constructor
**==========================
*/

Client::Client(int port, std::string const &ip) : AServer(port), ip(ip) {}

/*
**==========================
** create_socket - создание сокета и настройка адресов для установки соединения с сервером
**==========================
*/

void	Client::create_socket()
{
	if ((this->fd_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		Utils::print_error(0, "Esteblishing socket error (for server connection)");

	bzero(&this->server_addr, sizeof(this->server_addr));
	this->server_addr.sin_family		= AF_INET;
	this->server_addr.sin_port			= htons(this->port);
	inet_pton(AF_INET, DEF_HOST, &this->server_addr.sin_addr);

	Utils::print_line("Socket for server connection created");
}

/*
**==========================
** connection - установка соединения с сервером
**==========================
*/

void	Client::connection()
{
	if ((connect(this->fd_socket, reinterpret_cast<struct sockaddr *>(&this->server_addr), sizeof(this->server_addr))) == 0)
		std::cout << "Connection to server "
				  << inet_ntoa(server_addr.sin_addr)
				  << " with port number "
				  << this->port << " established\n";
	else
		Utils::print_error(0, "Unable to connect to server");
}

/*
**==========================
** send_message - отправка сообщения на сервер
**==========================
*/

void	Client::send_message()
{
	Utils::print_line("Sending message to server: " + (std::string)this->buffer);
	send(this->fd_socket, this->buffer, BUFFER_SIZE, 0);
}

/*
**==========================
** set_buffer - устанавливает значение буфера
**==========================
*/

void	Client::set_buffer(char const *buffer)
{
	strcpy(this->buffer, buffer);
}