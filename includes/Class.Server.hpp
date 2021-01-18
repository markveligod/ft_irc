#pragma once
/*
**==========================
** Class Server -
** Обмен информацией о состоянии соединения сервер - сервер:
**  - все знают другие сервера; 
**  - все знают пользовательскую информацию; 
**  - все знают информацию каналов.
**==========================
*/

#include "main.hpp"
#include "Class.User.hpp"
#include "Class.Channel.hpp"

class Server
{
	private:
		int						_fd_socket;
		string					_server_name;
		string					_host_name;
		int						_hopcount;
		string					_info;
		vector<User*>			_users;
		vector<Client*>			_clients;
		// vector<Channel>		_channels;
		string					_buffer;
		std::map<string, int>	map_cmd;

		Server();
		Server(const Server& other);
		Server& operator=(const Server& other);

	public:
		Server(int fd, const string& servername, int hopcount, const string& info);

		int					getSocketFd() const;
		int					getHopcount() const;
		const string &		getInfo() const;
		const string &		getName() const;
		const string &		getBuffer() const;
		// vector<User*>&		getUsers();
		// vector<Channel>&	getChannels();
		void				setBuffer(const string& buffer);
		void				addUser(User* new_user);
		void				addClient(Client* new_client);
		string 				get_line_break();
		bool				find_line_break();
		void				fill_map_cmd(string const & cmd_name);
};

