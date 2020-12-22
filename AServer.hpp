#ifndef ASERVER_HPP
# define ASERVER_HPP

#include "./main.hpp"

class AServer
{
protected:
	int					port;
	int					fd_socket;
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