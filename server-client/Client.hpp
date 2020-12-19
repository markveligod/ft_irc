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

#endif