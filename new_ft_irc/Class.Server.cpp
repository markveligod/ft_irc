#include "./Class.Server.hpp"

Server::Server() {}

Server::Server(std::string ip_network,  int port_network,  std::string pass_network,  int port_curr, std::string pass_curr)
{
    Utils::print_line("Constructor Server done!");
    this->ip_network = ip_network;
    this->port_network = port_network;
    this->pass_network = pass_network;
    this->port_curr = port_curr;
    this->pass_curr = pass_curr;
}

void Server::create_socket_locale()
{
    this->server_locale = Socket(this->ip_network.c_str(), this->port_curr);
    this->server_locale._bind();
    this->server_locale._listen();
}

void Server::create_socket_network()
{
    this->server_network = Socket(this->ip_network.c_str(), this->port_network);
    this->server_network._connect();
}

