#include "ClassServer.hpp"

Server::Server()
{
    this->FdServer = socket(AF_INET, SOCK_STREAM, 0);
    if (this->FdServer < 0)
    {
        std::cerr << "[SERVER]: " << RED << "ERROR: " << RESET << "code: " << ERR_FDSERVER << YELLOW << "Socket don't work\n" << RESET;
        exit(ERR_FDSERVER);
    }
    std::cout << "[SERVER]: " << GREEN << "Socker was created\n" << RESET;
}

Server::Server(const Server& other)
{
    *this = other;
}

Server::~Server()
{
    std::cout << "[SERVER]: " << RED << "Destroy\n" << RESET;
}

Server &Server::operator=(const Server& other)
{
    this->FdServer = other.FdServer;
    return (*this);
}
