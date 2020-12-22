#ifndef ASERVER_HPP
# define ASERVER_HPP

# include "./main.hpp"
# define BUFFER_SIZE 1024

class AServer
{
protected:
	int					port;
	int					fd_socket;
	char				buffer[BUFFER_SIZE];
	struct sockaddr_in	server_addr;

public:
	AServer(int port);

	virtual void		create_socket() = 0;
	virtual void		connection() = 0;
	virtual void		send_message() = 0;
	virtual bool		end_connection();
};

#endif