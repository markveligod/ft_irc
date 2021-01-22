#include "Class.Server.hpp"

Server::Server(int fd, const std::string& servername, int hopcount, const std::string& info)
{
    _fd_socket = fd;
    _server_name = servername;
    _hopcount = hopcount;
    _info = info;
}

bool Server::
find_line_break()
{
	if (_buffer.find("\n") != string::npos)
		return true;
	return false;
}

string Server::
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

int Server::
getSocketFd() const				{ return _fd_socket; }

const std::string& Server::
getName() const           		{ return _server_name;}

int Server::
getHopcount() const				{ return _hopcount; }

const std::string& Server::
getInfo() const					{ return _info; }

const string& Server::
getBuffer() const				{ return _buffer; }

Statistics& Server::
getStatistics()					{ return _statistics; }

// vector<Channel>& Server::
// getChannels() 					{ return (_channels); }

// vector<User*>& Server::
// getUsers()						{ return (_users); }

void Server::
setBuffer(const string& buffer) { _buffer = buffer; }

void Server::
addUser(User* new_user)			{ _users.push_back(new_user); }

void Server::
addClient(Client* new_client)	{ _clients.push_back(new_client); }
