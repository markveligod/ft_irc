#include "../includes/Class.Client.hpp"

Client::Client(int socket_fd) : socket_fd(socket_fd),
								password(false) {}

Client::Client(Client const &src) : nickname(src.nickname),
									socket_fd(src.socket_fd),
									hopcount(src.hopcount),
									password(src.password) {}
									

void Client::setNickname(std::string const &nickname)
{
	if (this->password == true)
		this->nickname = nickname;
}

void Client::setHopcount(int hopcount)
{
	if (this->password == true)
		this->hopcount = hopcount;
}

void Client::setPassword()
{
	this->password = true;
}

int Client::getHopcount() const
{
	return (this->hopcount);
}

bool Client::getPassword() const
{
	return (this->password);
}

int Client::getSocketFd() const
{
	return (this->socket_fd);
}

std::string const &Client::getNickname() const
{
	return (this->nickname);
}
