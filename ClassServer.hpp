#ifndef CLASSSERVER_HPP
# define CLASSSERVER_HPP

#pragma once

#include "./AServer.hpp"

class Server: public AServer
{
    private:
        int server;
        socklen_t size;

        Server();
    public:
        Server(const std::string &port_server);

        virtual void    create_socket();
	    virtual void    connection();
        virtual void	send_message();
	    void            recv_message();
        void            socket_listen();
        void            socket_accept();
        bool            check_fd_server();
	    std::string const get_buffer();
};

#endif