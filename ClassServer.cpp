#include "ClassServer.hpp"

Server::Server()
{
    this->FdServer = socket(AF_INET, SOCK_STREAM, 0);
    if (this->FdServer < 0)
        exit_error(ERR_FDSERVER, "Socket fail!");
    print_line("Socket was created");
}

Server::Server(const Server& other)
{
    *this = other;
}

Server::~Server()
{
    print_line("Destroy");
}

Server &Server::operator=(const Server& other)
{
    this->FdServer = other.FdServer;
    return (*this);
}
