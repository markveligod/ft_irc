#pragma once

#include "./AServer.hpp"

class Server: public AServer
{
    private:
        std::string HostIp4_network;
        int port_network;
        std::string pass_network;
        std::string password;

        Server();
    public:
        Server(const std::vector<std::string> &network, const std::string &port, const std::string &password);

        virtual void create_socket();
	    virtual void connection();
	    virtual void send_message();
};