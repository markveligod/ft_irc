#include "ClassServer.hpp"

Server::Server(const std::vector<std::string> &network, const std::string &port, const std::string &password)
{
    this->HostIp4_network = network[0];
    this->port_network = std::stoi(network[1]);
    this->pass_network = network[2];
    this->port = std::stoi(port);
    this->password = password;
    Utils::print_line("Constructor server Done!");
}

/*
**==========================
** create_socket
**==========================
*/

void Server::create_socket()
{

}

void Server::connection()
{

}

void Server::send_message()
{

}