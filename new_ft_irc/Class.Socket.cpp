#include <cstring>
#include "Class.Socket.hpp"

Socket::Socket() {}

Socket::Socket(const char *host_ip, int port, int fd, int sin_family, int type, int protocol)
				: _port(port), _fd(fd), _sin_family(sin_family),
				  _type(type), _protocol(protocol) {

	_addr_size = sizeof(_addr);
	
	bzero(&_addr, _addr_size);
	_addr.sin_family = _sin_family;
	if (host_ip)
		inet_pton(_sin_family, host_ip, &_addr.sin_addr);
	else
		_addr.sin_addr.s_addr = htons(INADDR_ANY);
	_addr.sin_port = htons(_port);
}

const Socket &Socket::operator=(const Socket &other)
{
	this->_port = other._port;
	this->_fd = other._fd;
	this->_sin_family = other._sin_family;
	this->_type = other._type;
	this->_protocol = other._protocol;
	this->_addr = other._addr;
	this->_addr_size = other._addr_size;
	return (*this);
}

int Socket::_socket() {

	if ((_fd = socket(_sin_family, _type, _protocol)) < 0)
		Utils::exit_error(ERR_SOCKET, "Establishing socket error");
	return _fd;
}

int Socket::_bind() {
	
	int res = 0;

	if ((res = bind(_fd, reinterpret_cast<struct sockaddr *>(&_addr), _addr_size)) < 0)
		Utils::exit_error(ERR_BIND, "Binding error");
	return res;
}

int Socket::_connect() {
	
	int res = 0;

	if ((res = connect(_fd, reinterpret_cast<struct sockaddr *>(&_addr), _addr_size)) < 0)
		Utils::exit_error(ERR_CONNECT_TO_SERVER, "Unable to connect to server");
	return res;
}

int Socket::_listen(int backlog) {

	int res = 0;

	if ((res = listen(_fd, backlog)) < 0)
		Utils::exit_error(ERR_LISTEN, "Listening error");
    Utils::print_line("Listening...");
	return res;
}

int Socket::_accept(struct sockaddr *addr, socklen_t *__restrict addrlen) {
	
	int client = 0;

	if ((client = accept(_fd, addr, addrlen)) < 0)
		Utils::exit_error(ERR_ACCEPT, "Accepting error");
	return client;
}

	// size_t	Socket::_send() { }

	// size_t	Socket::_recv() { }

	Socket::~Socket() {
		if (_fd)
			close(_fd);
	}