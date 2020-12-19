#ifndef CLIENT_HPP
# define CLIENT_HPP

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

class Client
{
private:
	int					port;
	int					client;
	std::string			ip;
	struct sockaddr_in	server_addr;
	char				buffer[BUFFER_SIZE];

public:
	Client();
	Client(int port, std::string const & ip);

	static bool			end_connection(const char *line);
	static void 		error(std::string const &str);

	void				create_socket();
	void				connection();
	void				chat();
	void				close_connection();
};

Client::Client() : port(SERVER_PORT), ip(SERVER_IP) {}

Client::Client(int port, std::string const & ip) : port(port), ip(ip) {}

bool			Client::end_connection(const char *line)
{
	std::string str(line);

	if (str == "end connection" || str == "end connectin\n")
		return (true);
	return (false);
}

void			Client::error(std::string const &str)
{
	std::cout << str << std::endl;
	exit(0);
}

void			Client::create_socket()
{
	if ((this->client = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		Client::error("Esteblishing socket error");

	bzero(&this->server_addr, sizeof(this->server_addr));
	this->server_addr.sin_family = AF_INET;
	this->server_addr.sin_port = htons(SERVER_PORT);
	inet_pton(AF_INET, SERVER_IP, &this->server_addr.sin_addr);

	std::cout << "Client socket created\n";
}

void			Client::connection()
{
	if ((connect(this->client, reinterpret_cast<struct sockaddr *>(&this->server_addr), sizeof(this->server_addr))) == 0)
		std::cout << "Connection to server...\n"
				  << inet_ntoa(server_addr.sin_addr)
				  << " with port number "
				  << this->port << std::endl;
	else
		Client::error("Connection error");

	recv(this->client, this->buffer, BUFFER_SIZE, 0);
	std::cout << "Connection established!" << std::endl;
}

void			Client::chat()
{
	while (1)
	{
		std::cout << "Client: ";
		std::cin.getline(this->buffer, BUFFER_SIZE);
		send(this->client, this->buffer, BUFFER_SIZE, 0);
		if (Client::end_connection(this->buffer))
			break;

		std::cout << "Server: ";
		recv(this->client, this->buffer, BUFFER_SIZE, 0);
		std::cout << this->buffer << std::endl;
		if (Client::end_connection(this->buffer))
			break;
	}
}

void			Client::close_connection()
{
	close(this->client);
	std::cout << "\nEnd\n";
}

#endif