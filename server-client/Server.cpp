#include "Server.hpp"

// --------------------------------Constructors--------------------------------

Server::Server() : port(SERVER_PORT) {}
Server::Server(int port) : port(port) {}


// ---------------------------Creating connection------------------------------

void			Server::create_socket()
{
	if ((this->client = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		Server::error("Esteblishing socket error");

	bzero(&this->server_addr, sizeof(this->server_addr));
	this->server_addr.sin_family 		= AF_INET;
	this->server_addr.sin_addr.s_addr 	= htons(INADDR_ANY);
	this->server_addr.sin_port			= htons(SERVER_PORT);

	std::cout << "Socket created\n";
}

void			Server::connection()
{
	socklen_t size = sizeof(this->server_addr);

	if (bind(this->client, reinterpret_cast<struct sockaddr*>(&this->server_addr), sizeof(this->server_addr)) < 0)
		Server::error("Binding error");
	std::cout << "Socket binded\n";
	if (listen(this->client, 10) < 0)
		Server::error("Listening error");
	std::cout << "Listening...\n";
	if ((this->server = accept(this->client, reinterpret_cast<struct sockaddr*>(&this->server_addr), &size)) < 0)
		Server::error("Accepting error");
	std::cout << "Connection accepted!\n";
}

void			Server::recv_message()
{
	std::cout << "Client: ";
	recv(this->server, _buffer, BUFFER_SIZE, 0);
	std::cout << _buffer << std::endl;
	if (Server::end_connection(_buffer))
		return ;
}

void			Server::chat()
{
	strcpy(_buffer, "Server connected!\n");
	send(this->server, _buffer, BUFFER_SIZE, 0);
	std::cout << "Connected to the client 1\n";

	std::cout << "Client: ";
	recv(this->server, _buffer, BUFFER_SIZE, 0);
	std::cout << _buffer << std::endl;
	if (Server::end_connection(_buffer))
		return ;

	/*while (1)
	{
		std::cout << "Server: ";
		std::cin.getline(_buffer, BUFFER_SIZE);
		send(this->server, _buffer, BUFFER_SIZE, 0);
		if (Server::end_connection(_buffer))
			break;

		std::cout << "Client: ";
		recv(this->server, _buffer, BUFFER_SIZE, 0);
		std::cout << _buffer << std::endl;
		if (Server::end_connection(_buffer))
			break;
	}*/
	std::cout << "\nEnd\n";
}


// --------------------------Helpful functions----------------------------

bool Server::end_connection(const char* line)
{
	std::string str(line);

	if (str == "end connection" || str == "end connectin\n")
		return true;
	return false;
}

void Server::error(const std::string& str)
{
	std::cout << str << std::endl;
	exit(0);
}