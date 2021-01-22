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
# include <cstring>			// for bzero and strcpy in Linux

# define SERVER_PORT	1280
# define BUFFER_SIZE	512
# define SERVER_IP		"127.0.0.1"

class Client
{
private:
	int					port;
	int					client;
	std::string			ip;
	struct sockaddr_in	server_addr;
	char				buffer[BUFFER_SIZE];
	std::string			pass;

public:
	Client();
	Client(int port, const std::string& ip, const std::string& pass);

	static bool			end_connection(const char* line);
	static void 		error(const std::string& str);

	void				create_socket();
	void				connection();
	void				connection_server();
	void				chat();
	void				close_connectionion();
	void				send_to_server();
	void set_buffer(char const* buffer) { strcpy(_buffer, buffer);};
};

#endif