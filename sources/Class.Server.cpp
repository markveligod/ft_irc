#include "Class.Server.hpp"

Server::Server(int fd, const std::string& servername, int hopcount, const std::string& info)
{
    this->_fd_socket = fd;
    this->_server_name = servername;
    this->_hopcount = hopcount;
    this->_info = info;
}

bool Server::
find_line_break()
{
	if (this->_buffer.find("\n") != string::npos)
		return (true);
	return (false);
}

string Server::
get_line_break()
{
	string temp_1;
	string temp_2;
	size_t pos;

	if ((pos = this->_buffer.find("\n")) == string::npos)
	{
		temp_1 = this->_buffer;
		this->_buffer.clear();
		return (temp_1);
	}
	if ((pos = this->_buffer.find("\r\n")) != string::npos)
	{
		temp_1 = string(this->_buffer.begin(), (this->_buffer.begin() + pos));
		temp_2 = string(this->_buffer.begin() + (pos + 2), this->_buffer.end());
	}
	else if ((pos = this->_buffer.find("\n")) != string::npos)
	{
		temp_1 = string(this->_buffer.begin(), (this->_buffer.begin() + pos));
		temp_2 = string(this->_buffer.begin() + (pos + 1), this->_buffer.end());
	}
	this->_buffer = temp_2;
	return (temp_1);
}

int Server::
getSocketFd() const				{ return(this->_fd_socket); }

const std::string& Server::
getServerName() const           { return (this->_server_name);}

int Server::
getHopcount() const				{ return (this->_hopcount); }

const std::string& Server::
getInfo() const					{ return (this->_info); }

const string& Server::
getBuffer() const				{ return (this->_buffer); }

// vector<Channel>& Server::
// getChannels() 					{ return (this->_channels); }

// vector<User*>& Server::
// getUsers()						{ return (this->_users); }

void Server::
setBuffer(string const &buffer) { this->_buffer = buffer; }

void Server::
addUser(User* new_user)			{ this->_users.push_back(new_user); }

void Server::
addClient(Client* new_client)	{ this->_clients.push_back(new_client); }
