#pragma once 

/*
**==========================
** Class IRC - 
**==========================
*/
#include <openssl/ssl.h>
#include <openssl/conf.h>
#include <openssl/evp.h>
#include <openssl/err.h>
#include "main.hpp"
#include "Class.Socket.hpp"
#include "Class.Command.hpp"
#include "Class.User.hpp"
#include "Class.Channel.hpp"
#include "Class.Server.hpp"

using std::string;

class Channel;

class IRC
{
	private:
		Socket						_network;
		string						_network_ip;
		int							_network_port;
		string						_network_pass;

		Socket						_localhost;
		Socket						_localhost_ssl;
		string						_localhost_pass;

		std::map<int, int>			_array_fd_select;
		fd_set						_fd_set_sockets;

		std::vector<User *>			_users;
		std::vector<Client *>		_clients;
		std::vector<Server *>		_servers;
		std::map<string, Channel>	_channels;

		int							_select_res;
		SSL*						_ssl;
		SSL_CTX*					_ctx;

	public:
		IRC();
		IRC(string network_ip,
			string network_port,
			string network_pass,
			string current_port,
			string _current_pass);

		IRC&		operator=(const IRC &other);

		void		create_socket_network();
		void		create_socket_local();

		void		init_fd_select();
		void		do_select();
		void		check_fd_select();
		void		do_command(Command * command, int socket_fd);

		void		init_ssl();
		void		init_ctx();
		SSL*		ssl_connection(int fd);

		int 		_send(int, int, const char *, size_t, int);
		int 		_recv(int, int, char *, size_t, int);

		template <typename T>
		static int	find_fd(std::vector<T> *vect, int fd);
		template <typename T>
		static int	find_nickname(std::vector<T> *vect, string const & nickname);
		User*		get_user(string nickname);
		
		void		delete_user(int fd);
		void		delete_client(int fd);

		std::vector<string> check_buffer(int fd, const char *buffer);
		//static int	find_fd(std::vector<Client *> *vect, int fd);
		//static int	find_fd(std::vector<User *> *vect, int fd);
		void		join_channel(string, string);
};