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
#include "Class.Statistics.hpp"

typedef std::map<string, Channel, utils::less>				channel_map;
typedef std::map<string, Channel, utils::less>::iterator	channel_iterator;

using std::map;
using std::string;
using std::vector;
using std::pair;
using std::queue;
using utils::is_equal;

class Channel;
class Command;
class Statistics;

class IRC
{
	private:
		string						_server_name;
		string						_host_name;

		vector<Socket>				_network;
		vector<string>				_network_ip;
		vector<int>					_network_port;
		vector<string>				_network_pass;

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
		channel_map					_channels;

		int							_select_res;
		SSL*						_ssl;
		SSL_CTX*					_ctx;

		Statistics					_statistics;

	public:
		IRC();

		IRC(string network_ip,
			string network_port,
			string network_pass,
			string localhost_port,
			string localhost_pass,
			string operator_user = OPER_MASK,
			string operator_pass = OPER_PASS);

		~IRC();

		IRC&						operator=(const IRC& other);

		void						create_socket_network();
		void						create_socket_local();
		void						init_fd_select();
		void						do_select();
		void						check_fd_select();
		void						do_command(Command* command, int socket_fd);

		void						init_ssl();
		void						init_ctx();
		SSL*						ssl_connection(int fd);

		int 						_send(int, int, const char*, size_t, int);
		int 						_recv(int, int, char*, size_t, int);
		
		void						add_network_ip(const string& ip);
		void						add_network_port(const string& port);
		void						add_network_pass(const string& pass);
		void						add_fd(int fd, int fd_type);

		void						delete_user(int fd);
		void						delete_client(int fd);
		void						delete_channel(string channel_name);
		void						delete_client(Client* clnt);
		void						delete_user(User* user);
		void						delete_user_from_channels(User* user, const string& quit_mess);
		void 						close_connection(int fd, int n);
		void 						close_connection(Client* client);
		void 						close_connection(User* user);
		void 						close_connection(Server* server);

		vector<string> 				check_buffer(int fd, const char* buffer);

		User*						get_user(int fd);
		User*						get_user(string nickname);
		User*						get_user(Client* client);
		vector<User*>& 				get_users();
		vector<Client*>& 			get_clients();
		Client*						get_client(int fd);
		Client*						get_client(User* user);
		Client*						get_client(Server* user);
		vector<Server*>& 			get_servers();
		Server*						get_server(int fd);
		Server*						get_server(const string& name) const;
		const string& 				get_server_name();
		const string& 				get_server_name(int fd);
		size_t						get_server_count();
		const string&				get_nickname(int fd);
		int							get_localhost_port() const;
		const string&				get_localhost_pass() const;
		const string&				get_operator_user() const;
		const string&				get_operator_pass() const;
		const Socket&				get_socket() const;
		channel_map&				get_channels();
		Channel*					get_channel(string channel_name);
		Statistics &				get_statistics();
		vector<Socket>&				get_network();
		bool						is_empty_queue() const;
		bool						is_server(int fd) const;
		bool						is_server_operator(const User*) const;
		bool						is_numeric_response(const Command& command);

		template <typename T>
		static int					find_fd(T& container, int fd);
		template <typename T>
		static int					find_name(T& container, const string& nickname);
		template <typename T>
		static int 					find_fd_count(T &cont, int fd);

		void 						push_cmd_queue(int fd, const string& str);
		string						fullname(const User*) const;
		string						response(int response_code, int client_fd, string command, string message);
		string						response(int response_code, string name, string command, string message);
		void						forward_to_servers(int fd, const string& message);
		void						forward_to_clients(IRC& irc, const string& message);
		void						forward_to_channel(int fd, Channel& channel, const string& message);
		void						forward_to_channel(int fd, const string& channel_name, const string& message);
		void						forward_to_all_channels(User* user, const string& message);

		vector<string>				motd_generate();

		void print_channels() const; //DEBUG
};

#include "Class.IRC.templates.tpp"