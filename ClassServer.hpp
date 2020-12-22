#pragma once

#include "./AServer.hpp"

class Server: public AServer
{
    private:
        int server;
        std::string HostIp4_network;
        int port_network;
        std::string pass_network;
        std::string password;

        Server();
    public:
        Server(const std::string &port_server);

        virtual void    create_socket();
	    virtual void    connection();
	    void            chat();
	    void            recv_message();
	    std::string const get_buffer();
};