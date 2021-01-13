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

using std::map;
using std::string;
using std::vector;
using std::pair;
using std::queue;

class Channel;
class Command;

class IRC
{
	private:
		string						_server_name;

		Socket						_network;
		string						_network_ip;
		int							_network_port;
		string						_network_pass;

		Socket						_localhost;
		Socket						_localhost_ssl;
		string						_localhost_pass;
		string						_operator_user;
		string						_operator_pass;

		map<int, int>				_array_fd_select;
		queue<pair<int, string> >	_command_queue;
		fd_set						_fd_set_read;
		fd_set						_fd_set_write;

		vector<User*>				_users;
		vector<Client*>				_clients;
		vector<Server*>				_servers;
		map<string, Channel>		_local_channels;
		map<string, Channel>		_shared_channels;

		int							_select_res;
		SSL*						_ssl;
		SSL_CTX*					_ctx;

	public:
		IRC();
		IRC(string network_ip,
			string network_port,
			string network_pass,
			string localhost_port,
			string localhost_pass,
			string operator_user = "foo",
			string operator_pass = "bar");
		~IRC();

		IRC&						operator=(const IRC& other);

		void						create_socket_network();
		void						create_socket_local();
		void						init_fd_select();
		void						do_select();
		void						check_fd_select();
		int							do_command(Command* command, int socket_fd);

		void						init_ssl();
		void						init_ctx();
		SSL*						ssl_connection(int fd);

		int 						_send(int, int, const char*, size_t, int);
		int 						_recv(int, int, char*, size_t, int);
		
		void						delete_user(int fd);
		void						delete_client(int fd);
		void						delete_channel(string channel_name, char type);

		vector<string> 				check_buffer(int fd, const char* buffer);

		User*						get_user(string nickname);
		User*						get_user(int fd);
		vector<User*>& 				get_users();
		vector<Client*>& 			get_clients();
		vector<Server*>& 			get_servers();
		const string& 				get_server_name();
		int							get_localhost_port() const;
		const string&				get_localhost_pass() const;
		const string&				get_operator_user() const;
		const string&				get_operator_pass() const;
		const Socket&				get_socket() const;
		map<string, Channel>&		get_local_channels();
		map<string, Channel>&		get_shared_channels();
		Channel*					get_channel(string channel_name);
		bool						isEmptyQuene() const;

		template <typename T>
		static int					find_fd(T& container, int fd);
		template <typename T>
		static int					find_nickname(T& container, const string& nickname);
		string						full_name(const User*) const;

		void 						push_cmd_queue(int fd, const string& str);
		void 						close_connect(int fd, int n);
		
		string						response_to_client(int response_code, int client_fd, string message_prefix, string message);

		void print_channels() const;
};

#include "../Class.IRC.templates.cpp"