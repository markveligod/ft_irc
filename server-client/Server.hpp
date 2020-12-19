#ifndef SERVER_HPP
# define SERVER_HPP

# include <iostream>
# include <string>
# include <stdlib.h>
# include <unistd.h>
# include <arpa/inet.h>
# include <sys/socket.h>
# include <sys/ioctl.h>
# include <fcntl.h>
# include <netdb.h>
# include <netinet/in.h>

# define SERVER_PORT	1080
# define BUFFER_SIZE	1024
# define SERVER_IP		"127.0.0.1"

class Server
{
private:
	int					port;
	int					server;
	int					client;
	struct sockaddr_in	server_addr;
	char				buffer[BUFFER_SIZE];

public:
	Server();
	Server(int port);

	static bool			end_connection(const char *line);
	static void 		error(std::string const &str);

	void				create_socket();
	void				connection();
	void				chat();
};

Server::Server() : port(SERVER_PORT) {}

Server::Server(int port) : port(port) {}

bool			Server::end_connection(const char *line)
{
	std::string str(line);

	if (str == "end connection" || str == "end connectin\n")
		return (true);
	return (false);
}

void			Server::error(std::string const &str)
{
	std::cout << str << std::endl;
	exit(0);
}

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

	if (bind(this->client, reinterpret_cast<struct sockaddr *>(&this->server_addr), sizeof(this->server_addr)) < 0)
		Server::error("Binding error");
	std::cout << "Socket binded\n";
	if (listen(this->client, 10) < 0)
		Server::error("Listening error");
	std::cout << "Listening...\n";
	if ((this->server = accept(this->client, reinterpret_cast<struct sockaddr *>(&this->server_addr), &size)) < 0)
		Server::error("Accepting error");
	std::cout << "Connection accepted!\n";
}

void			Server::chat()
{
	strcpy(this->buffer, "Server connected!\n");
	send(this->server, this->buffer, BUFFER_SIZE, 0);
	std::cout << "Connected to the client 1\n";

	std::cout << "Client: ";
	recv(this->server, this->buffer, BUFFER_SIZE, 0);
	std::cout << this->buffer << std::endl;
	if (Server::end_connection(this->buffer))
		return ;

	while (1)
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
	}
	std::cout << "\nEnd\n";
}

#endif