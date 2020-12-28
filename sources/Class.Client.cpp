#include "Class.Client.hpp"

/*
** ----------------------------------------------------------
** Constructors
** ----------------------------------------------------------
*/

Client::Client(int socket_fd) : socket_fd(socket_fd),
								password(false) {}

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

/*
** ----------------------------------------------------------
** Набор функций для парсинга незавершенных команд:
** setBuffer - устанавливает буфер
** find_line_break - находит перенос строки \n или \r
** get_line_break - если find_line_break возвращает true то парсится команда до \n или \r
** isempty_buffer - проверяет пустой ли буфер клиента и возвращает булевое значение
** ----------------------------------------------------------
*/

void Client::setBuffer(const std::string &buff)
{
	this->buffer = buff;
}

bool Client::find_line_break()
{
	if (this->buffer.find("\r") != std::string::npos || this->buffer.find("\n") != std::string::npos)
		return (true);
	return (false);
}

std::string Client::get_line_break()
{
	std::string temp_1;
	std::string temp_2;
	size_t pos;

	if ((pos = this->buffer.find("\r")) == std::string::npos)
	{
		if ((pos = this->buffer.find("\n")) == std::string::npos)
		{
			temp_1 = this->buffer;
			this->buffer.clear();
			return (temp_1);
		}
	}
	temp_1 = std::string(this->buffer.begin(), (this->buffer.begin() + pos));
	temp_2 = std::string(this->buffer.begin() + (pos + 1), this->buffer.end());
	this->buffer = temp_2;
	return (temp_1);
}

bool Client::isempty_buffer()
{
	return (this->buffer.empty());
}

