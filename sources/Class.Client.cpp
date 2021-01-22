#include "../includes/Class.Client.hpp"

using std::string;

/*
** ----------------------------------------------------------
** Constructors
** ----------------------------------------------------------
*/

Client::
Client(const string& hostname, int socket_fd, int hopcount) 		
					  : _nickname(""),
					  	_hostname(hostname),
					  	_socket_fd(socket_fd),
						_hopcount(hopcount),
						_password(false),
						_is_server(false) {}

Client::
Client(int socket_fd, const string&  nickname, const string& hostname, int hopcount)
									: _nickname(nickname),
									  _hostname(hostname),
									  _socket_fd(socket_fd),
									  _hopcount(hopcount),
									  _password(true),
									  _is_server(false) {}

Client::
Client(const Client& src)
					  : _nickname(src._nickname),
					  	_hostname(src._hostname),
						_socket_fd(src._socket_fd),
						_hopcount(src._hopcount),
						_password(src._password),
						_is_server(src._is_server) {}

Client& Client::
operator=(const Client& src)
{
	_nickname = src._nickname;
	_hostname = src._hostname;
	_socket_fd = src._socket_fd;
	_hopcount = src._hopcount;
	_password = src._password;
	_buffer = src._buffer;
	_is_server = src._is_server;
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
	if (_password == true)
		_nickname = nickname;
}

void Client::
setHopcount(int hopcount)
{
	if (_password == true)
		_hopcount = hopcount;
}

void Client::
setPassword(bool password)		{ _password = password; }

void Client::
setIsServer(bool state)			{ _is_server = state; }

/*
** ----------------------------------------------------------
** Getters
** ----------------------------------------------------------
*/

int Client::
getHopcount() const				{ return (_hopcount); }

bool Client::
getPassword() const				{ return (_password); }

int Client::
getSocketFd() const				{ return (_socket_fd); }

const string& Client::
getName() const					{ return (_nickname); }

const string&	Client::
getBuffer() const				{ return (_buffer); }

bool			Client::
getIsServer() const				{ return  (_is_server); }

const string& Client::
getHostname() const				{ return (_hostname); }

Statistics &Client::
getStatistics() 				{ return (_statistics); }


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
	_buffer = buff;
}

bool Client::
find_line_break()
{
	if (_buffer.find("\n") != string::npos)
		return true;
	return false;
}

string Client::
get_line_break()
{
	string temp_1;
	string temp_2;
	size_t pos;

	if ((pos = _buffer.find("\n")) == string::npos)
	{
			temp_1 = _buffer;
			_buffer.clear();
			return (temp_1);
	}
	if ((pos = _buffer.find("\r\n")) != string::npos)
	{
		temp_1 = string(_buffer.begin(), (_buffer.begin() + pos));
		temp_2 = string(_buffer.begin() + (pos + 2), _buffer.end());
	}
	else if ((pos = _buffer.find("\n")) != string::npos)
	{
		temp_1 = string(_buffer.begin(), (_buffer.begin() + pos));
		temp_2 = string(_buffer.begin() + (pos + 1), _buffer.end());
	}

	_buffer = temp_2;
	return (temp_1);
}

bool Client::
isempty_buffer()			{ return (_buffer.empty()); }

