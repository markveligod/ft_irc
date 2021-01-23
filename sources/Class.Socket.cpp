#include <cstring>
#include "Class.Socket.hpp"

Socket::
Socket() {}

Socket::
Socket(const char* host_ip,
		int port,
		int fd,
		int sin_family,
		int type,
		int protocol)
			  : _port(port),
				_fd(fd),
				_sin_family(sin_family),
				_type(type),
				_protocol(protocol)
{
	_addr_size = sizeof(_addr);
	bzero(&_addr, _addr_size);
	_addr.sin_family = _sin_family;

	if (host_ip)
	{
		hostent* host = gethostbyname(host_ip);
		if (!host)
			utils::exit_error(1, "Unknown address");
		inet_pton(_sin_family, host->h_name, &_addr.sin_addr);
	}
	else
		_addr.sin_addr.s_addr = htons(INADDR_ANY);
	_addr.sin_port = htons(_port);
}

const Socket& Socket::
operator=(const Socket& other)
{
	_port			= other._port;
	_fd			= other._fd;
	_sin_family	= other._sin_family;
	_type			= other._type;
	_protocol		= other._protocol;
	_addr			= other._addr;
	_addr_size	= other._addr_size;
	return (*this);
}

int Socket::
_socket() {

	if ((_fd = socket(_sin_family, _type, _protocol)) < 0)
		utils::exit_error(ERR_SOCKET, "Establishing socket error");
	return _fd;
}

int Socket::
_bind() {
	
	int res = 0;

	if ((res = bind(_fd, reinterpret_cast<struct sockaddr*>(&_addr), _addr_size)) < 0)
		utils::exit_error(ERR_BIND, "Binding error");
	return res;
}

int Socket::
_connect() {
	
	int res = 0;

	if ((res = connect(_fd, reinterpret_cast<struct sockaddr*>(&_addr), _addr_size)) < 0)
		utils::print_error(ERR_CONNECT_TO_SERVER, "Unable to connect to server");
	fcntl(_fd, F_SETFL, O_NONBLOCK);
	return res;
}

int Socket::
_listen(int backlog) {

	int res = 0;

	if ((res = listen(_fd, backlog)) < 0)
		utils::exit_error(ERR_LISTEN, "Listening error");
	return res;
}

int Socket::
_accept() {
	
	int client = 0;

	if ((client = accept(_fd, reinterpret_cast<struct sockaddr*>(&_addr), &_addr_size)) < 0)
		utils::exit_error(ERR_ACCEPT, "Accepting error");
	return client;
}

Socket::
~Socket() {
	if (_fd)
		close(_fd);
}

int Socket::
get_fd() const		{ return _fd; }

int Socket::
get_port() const	{ return _port; }

const std::string Socket::
getHostname() const
{
	//std::string hostname(inet_ntoa(_addr.sin_addr));
	return (std::string(inet_ntoa(_addr.sin_addr)));
}
