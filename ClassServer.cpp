#include "ClassServer.hpp"

Server::Server(const std::string &port_server): AServer(stoi(port_server))
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
** connection - 
**==========================
*/

void Server::connection()
{
    socklen_t size = sizeof(this->server_addr);

	if (bind(this->fd_socket, reinterpret_cast<struct sockaddr *>(&this->server_addr), sizeof(this->server_addr)) < 0)
		Utils::exit_error(ERR_BIND, "Binding error");
    Utils::print_line("Socket binded!");
	if (listen(this->fd_socket, 10) < 0)
		Utils::exit_error(ERR_LISTEN, "Listening error");
    Utils::print_line("Listening...");
	if ((this->server = accept(this->fd_socket, reinterpret_cast<struct sockaddr *>(&this->server_addr), &size)) < 0)
		Utils::exit_error(ERR_ACCEPT, "Accepting error");
    Utils::print_line("Connection accepted!");
}

/*
**==========================
** chat - 
**==========================
*/

void Server::chat()
{
    strcpy(this->buffer, "Server connected!\n");
	send(this->server, this->buffer, BUFFER_SIZE, 0);
	Utils::print_line("Connected to the client 1");

	std::cout << "Client: ";
	recv(this->server, this->buffer, BUFFER_SIZE, 0);
	std::cout << this->buffer << std::endl;
	if (AServer::end_connection())
		return ;

	/*while (1)
	{
		std::cout << "Server: ";
		std::cin.getline(this->buffer, BUFFER_SIZE);
		send(this->server, this->buffer, BUFFER_SIZE, 0);
		if (Server::end_connection(this->buffer))
			break;

		std::cout << "Client: ";
		recv(this->server, this->buffer, BUFFER_SIZE, 0);
		std::cout << this->buffer << std::endl;
		if (Server::end_connection(this->buffer))
			break;
	}*/
    Utils::print_line("nEnd");
	std::cout << "\nEnd\n";
}

/*
**==========================
** recv_message - 
**==========================
*/

void Server::recv_message()
{
    std::cout << "Client: ";
	recv(this->server, this->buffer, BUFFER_SIZE, 0);
	std::cout << this->buffer << std::endl;
	if (AServer::end_connection())
		return ;
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