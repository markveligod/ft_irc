#include "Class.Server.hpp"

Server::Server(const int & fd)
{
    this->_fd_socket = fd;
}

Server::~Server()
{}

const int & Server::getFdSocket() const
{
    return(this->_fd_socket);
}

void Server::addUser(User *new_user)
{
    this->_users.push_back(new_user);
}