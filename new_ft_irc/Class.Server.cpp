#include "./Class.Server.hpp"

Server::Server() {}

Server::Server(std::string ip_network, std::string port_network,  std::string pass_network, std::string port_curr, std::string pass_curr)
{
    this->ip_network = ip_network;
    this->port_network = std::atoi(port_network.c_str());
    this->pass_network = pass_network;
    this->port_curr = std::atoi(port_curr.c_str());
    this->pass_curr = pass_curr;
    Utils::print_line("Constructor Server done!");
}

void Server::create_socket_locale()
{
    this->server_locale = Socket(this->ip_network.c_str(), this->port_curr);
    // this->server_locale = Socket(0, this->port_curr);
    Utils::print_line("Socket locale done!");
    this->server_locale._socket();
    Utils::print_line("Socket locale FD done!");
    this->server_locale._bind();
    Utils::print_line("Socket locale bind done!");
    this->server_locale._listen();
    Utils::print_line("Socket locale listen...");
}

void Server::create_socket_network()
{
    this->server_network = Socket(this->ip_network.c_str(), this->port_network);
    this->server_network._connect();
}

