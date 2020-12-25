/*
**==========================
** Class Socket - 
**==========================
*/
#pragma once

#include <netinet/in.h> //struct sockaddr_in
#include <sys/socket.h> //socket
#include "main.hpp"

class Socket
{
protected:
	int					_port;
	int					_fd;
	int					_sin_family;
	int					_type;
	int					_protocol;
	char				_buffer[BUFFER_SIZE];
	struct sockaddr_in	_addr;
	socklen_t			_addr_size;

public:
	Socket();
	Socket(const char *host_ip,
			int port, int fd = 0,
			int sin_family = AF_INET,
			int type = SOCK_STREAM,
			int protocol = 0);
	// Socket(const Socket&);
	
	~Socket();

	const Socket &operator=(const Socket &other);

	int		_socket();
	int		_bind();
	int		_connect();
	int		_listen(int backlog = 5);
	int		_accept();
	// int		_accept(struct sockaddr *addr = NULL, socklen_t *__restrict addrlen = NULL);
	// size_t	_send();
	// size_t	_recv();
};
