#ifndef CLASSSERVER_HPP
# define CLASSSERVER_HPP

# pragma once

# include "./AServer.hpp"
# include "ClassClient.hpp"
# define FD_FREE 0
# define FD_CLIENT 1
# define FD_SERVER 2

class Server: public AServer
{
    private:
        int			server;
        socklen_t	size;
		fd_set		client_sockets;
		int			socket_type[1024];
		int			select_res;

        Server();
    public:
        Server(const std::string &port_server);

        virtual void    	create_socket();
	    virtual void    	connection();
        virtual void		send_message();
	    void            	recv_message(int i);
        void           		socket_listen();
        void            	socket_accept();
		void				init_fd_select();
		void				do_select();
		void				check_fd_select(Client *client_part);
		bool            	check_fd_server();

	    std::string const	get_buffer();
};

#endif