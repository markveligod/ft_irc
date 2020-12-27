#include "Class.Client.hpp"

/*
** ----------------------------------------------------------
** Constructors
** ----------------------------------------------------------
*/

Client::Client(int socket_fd) : socket_fd(socket_fd),
								password(false),
								status_ready(false) {}

Client::Client(Client const &src) : nickname(src.nickname),
									socket_fd(src.socket_fd),
									hopcount(src.hopcount),
									password(src.password) {}

/*
** ----------------------------------------------------------
** Setters
** ----------------------------------------------------------
*/

void 	Client::setNickname(std::string const &nickname)
{
	if (this->password == true)
		this->nickname = nickname;
}

void 	Client::setHopcount(int hopcount)
{
	if (this->password == true)
		this->hopcount = hopcount;
}

void 	Client::setPassword()
{
	this->password = true;
}

void	Client::setUsername(std::string const &username)
{
	this->username = username;
}

void	Client::setHostname(std::string const &hostname)
{
	this->hostname = hostname;
}

void	Client::setServername(std::string const &servername)
{
	this->servername = servername;
}

void	Client::setRealname(std::string const &realname)
{
	this->realname = realname;
}

void	Client::setStatusReady(bool status)
{
	this->status_ready = status;
}

/*
** ----------------------------------------------------------
** Getters
** ----------------------------------------------------------
*/

int 				Client::getHopcount() const
{
	return (this->hopcount);
}

bool 				Client::getPassword() const
{
	return (this->password);
}

int 				Client::getSocketFd() const
{
	return (this->socket_fd);
}

std::string const &	Client::getNickname() const
{
	return (this->nickname);
}

std::string const &	Client::getUsername() const
{
	return (this->username);
}

std::string const &	Client::getHostname() const
{
	return (this->hostname);
}

std::string const &	Client::getServername() const
{
	return (this->servername);
}

std::string const &	Client::getRealname() const
{
	return (this->realname);
}

bool	Client::getStatusReady() const
{
	return (this->status_ready);
}
