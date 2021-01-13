#include "Class.Server.hpp"

Server::Server(int fd, const std::string& servername, int hopcount, const std::string& info)
{
    this->_fd_socket = fd;
    this->_server_name = servername;
    this->_hopcount = hopcount;
    this->_info = info;
}

int Server::
getSocketFd() const				{ return(this->_fd_socket); }

const std::string &Server::
getServerName() const           { return (this->_server_name);}

void Server::
addUser(User* new_user)			{ this->_users.push_back(new_user); }

void Server::
addClient(Client* new_client)	{ this->_clients.push_back(new_client); }