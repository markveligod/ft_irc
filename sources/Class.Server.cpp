#include "Class.Server.hpp"

Server::Server(const int & fd, const std::string & servername, const int & hopcount, const std::string & info)
{
    this->_fd_socket = fd;
    this->_server_name = servername;
    this->_hopcount = hopcount;
    this->_info = info;

    std::cout << "FD: " << fd << std::endl;
    std::cout << "servername: " << servername << std::endl;
    std::cout << "hopcount: " << hopcount << std::endl;
    std::cout << "info: " << info << std::endl;
    std::cout << "DONE!" << std::endl;
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