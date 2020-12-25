#ifndef SERVER_HPP
# define SERVER_HPP

# include <iostream>
# include <string>
# include <cstring> //bzero strcpy
# include <stdlib.h>
# include <unistd.h>
# include <arpa/inet.h>
# include <sys/socket.h>
# include <sys/ioctl.h>
# include <fcntl.h>
# include <netdb.h>
# include <netinet/in.h>

# define SERVER_PORT	1280
# define BUFFER_SIZE	512
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
	void				recv_message();
	std::string const get_buffer() { return (std::string(this->buffer)); };
};

#endif