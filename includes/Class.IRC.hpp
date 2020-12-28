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

class IRC
{
	private:
		Socket					_network;
		std::string				_network_ip;
		int						_network_port;
		std::string				_network_pass;
		Socket					_localhost;
		Socket					_localhost_ssl;
		std::string				_localhost_pass;
		std::map<int, int>		_array_fd_select;
		std::vector<User *>		_users;
		std::vector<Client *>	_clients;
		fd_set					_fd_set_sockets;
		int						_select_res;
		SSL*					_ssl;
		SSL_CTX*				_ctx;

	public:
		IRC();
		IRC(std::string network_ip,
			   std::string network_port,
			   std::string network_pass,
			   std::string current_port,
			   std::string _current_pass);

		IRC &		operator=(const IRC &other);

		void		create_socket_network();
		void		create_socket_local();

		void		init_fd_select();
		void		do_select();
		void		check_fd_select();
		void		do_command(Command * command, int socket_fd);

		void		init_ssl();
		void		init_ctx();
		SSL *		ssl_connection(int fd);

		int 		_send(int, int, const char *, size_t, int);
		int 		_recv(int, int, char *, size_t, int);

		template <typename T>
		static int	find_fd(std::vector<T> *vect, int fd);
		template <typename T>
		static int	find_nickname(std::vector<T> * vect, std::string const & nickname);
		
		void		delete_user(int fd);
		void		delete_client(int fd);
};