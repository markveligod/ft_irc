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
        std::string 			_server_name;
        int						_hopcount;
        std::string 			_info;
        std::vector<User*> 		_users;
		std::vector<Client*>	_clients;
		std::vector<Channel*> 	_channels;

		Server();
        Server(const Server& other);
        Server& operator=(const Server& other);

    public:
        Server(int fd, const std::string& servername, int hopcount, const std::string& info);

		int					getSocketFd() const;
		int					getHopcount() const;
		const std::string &	getInfo() const;
        const std::string &	getServerName() const;
		void				addUser(User* new_user);
		void				addClient(Client* new_client);
};

