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
        string 			_server_name;
        int						_hopcount;
        string 			_info;
        vector<User*> 		_users;
		vector<Client*>	_clients;
		vector<Channel*> 	_channels;
		string				_buffer;

		Server();
        Server(const Server& other);
        Server& operator=(const Server& other);

    public:
        Server(int fd, const string& servername, int hopcount, const string& info);

		int					getSocketFd() const;
		int					getHopcount() const;
		const string &		getInfo() const;
        const string &		getServerName() const;
		const string &		getBuffer() const;
		void				setBuffer(string const & buffer);
		void				addUser(User* new_user);
		void				addClient(Client* new_client);
		string 				get_line_break();
		bool				find_line_break();
};

