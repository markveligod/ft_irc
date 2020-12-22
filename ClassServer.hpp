#pragma once

#include "./AServer.hpp"

class Server: public AServer
{
    private:
        int server;

        Server();
    public:
        Server(const std::string &port_server);

        virtual void    create_socket();
	    virtual void    connection();
        virtual void	send_message();
	    void            recv_message();
	    std::string const get_buffer();
};