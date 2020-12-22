#ifndef ASERVER_HPP
# define ASERVER_HPP

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

# define SERVER_PORT	1777
# define BUFFER_SIZE	1024
# define SERVER_IP		"127.0.0.1"

class AServer
{
private:
	int					port;
	int					fd_client;
	char				buffer[BUFFER_SIZE];
	struct sockaddr_in	server_addr;

public:
	virtual void		create_socket() = 0;
	virtual void		connection() = 0;
	virtual void		send_message() = 0;
	virtual bool		end_connection();
};

bool AServer::end_connection()
{
	std::string str(this->buffer);

	if (str == "end connection" || str == "end connectin\n")
		return (true);
	return (false);
}

#endif