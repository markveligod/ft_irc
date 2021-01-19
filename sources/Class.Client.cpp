#include "../includes/Class.Client.hpp"

using std::string;

/*
** ----------------------------------------------------------
** Constructors
** ----------------------------------------------------------
*/

Client::
Client(const string& hostname, int socket_fd, int hopcount) 		
					  : nickname(""),
					  	hostname(hostname),
					  	socket_fd(socket_fd),
						hopcount(hopcount),
						password(false),
						is_server(false) {}

Client::
Client(int socket_fd, const string&  nickname, const string& hostname, int hopcount)
									: nickname(nickname),
									  hostname(hostname),
									  socket_fd(socket_fd),
									  hopcount(hopcount),
									  password(true),
									  is_server(false) {}

Client::
Client(const Client& src)
					  : nickname(src.nickname),
					  	hostname(src.hostname),
						socket_fd(src.socket_fd),
						hopcount(src.hopcount),
						password(src.password),
						is_server(src.is_server) {}

Client& Client::
operator=(const Client& src)
{
	nickname = src.nickname;
	hostname = src.hostname;
	socket_fd = src.socket_fd;
	hopcount = src.hopcount;
	password = src.password;
	buffer = src.buffer;
	is_server = src.is_server;
	return *this;
}

/*
** ----------------------------------------------------------
** Setters
** ----------------------------------------------------------
*/

void Client::
setNickname(const string& nickname)
{
	if (this->password == true)
		this->nickname = nickname;
}

void Client::
setHopcount(int hopcount)
{
	if (this->password == true)
		this->hopcount = hopcount;
}

void Client::
setPassword(bool password)		{ this->password = password; }

void Client::
setIsServer(bool state)			{ this->is_server = state; }

/*
** ----------------------------------------------------------
** Getters
** ----------------------------------------------------------
*/

int Client::
getHopcount() const				{ return (this->hopcount); }

bool Client::
getPassword() const				{ return (this->password); }

int Client::
getSocketFd() const				{ return (this->socket_fd); }

const string& Client::
getName() const					{ return (this->nickname); }

const string&	Client::
getBuffer() const				{ return (this->buffer); }

bool			Client::
getIsServer() const				{ return  (this->is_server); }

const string& Client::
getHostname() const				{ return (this->hostname); }

Statistics &Client::
getStatistics() 				{ return (this->statistics); }


/*
** ----------------------------------------------------------
** Набор функций для парсинга незавершенных команд:
** setBuffer - устанавливает буфер
** find_line_break - находит перенос строки \n или \r
** get_line_break - если find_line_break возвращает true то парсится команда до \n или \r
** isempty_buffer - проверяет пустой ли буфер клиента и возвращает булевое значение
** ----------------------------------------------------------
*/

void Client::
setBuffer(const string& buff)
{
	this->buffer = buff;
}

bool Client::
find_line_break()
{
	if (this->buffer.find("\n") != string::npos)
		return (true);
	return (false);
}

string Client::
get_line_break()
{
	string temp_1;
	string temp_2;
	size_t pos;

	if ((pos = this->buffer.find("\n")) == string::npos)
	{
			temp_1 = this->buffer;
			this->buffer.clear();
			return (temp_1);
	}
	if ((pos = this->buffer.find("\r\n")) != string::npos)
	{
		temp_1 = string(this->buffer.begin(), (this->buffer.begin() + pos));
		temp_2 = string(this->buffer.begin() + (pos + 2), this->buffer.end());
	}
	else if ((pos = this->buffer.find("\n")) != string::npos)
	{
		temp_1 = string(this->buffer.begin(), (this->buffer.begin() + pos));
		temp_2 = string(this->buffer.begin() + (pos + 1), this->buffer.end());
	}

	this->buffer = temp_2;
	return (temp_1);
}

bool Client::
isempty_buffer()			{ return (this->buffer.empty()); }

