
/*
**==========================
** Class Command - Класс реализует логику распарсинга данных по командам:
** PASS - используется для установки 'парольного соединения' 
** NICK - используется для установки пользовательского никнейма или изменения предыдущего
**==========================
*/

#pragma once
#include "main.hpp"
class IRC;
class Server;
#include "Class.User.hpp"
#include "Class.IRC.hpp"
#include "codes.hpp"

class Command
{
private:
	std::string						prefix;
	std::string						command;
	std::vector<std::string> 		arguments;


	bool							nick_valid();
	template <typename T> bool		nick_available(std::vector<T> vect, std::string const &nick);

	bool							pass(std::string password, std::string local_pass);
	void							user_change(User * curr_user);
	void							user_create(Client * curr_client, std::vector<User *> &users, Server * curr_server);
	std::vector<Client *>::iterator find_fd(std::vector<Client *> *vect, int fd);
	
public:
	Command(std::string const & str);
	~Command();

	int					cmd_nick(IRC& irc, int fd);
	int					cmd_pass(IRC& irc, int fd);
	int					cmd_user(IRC& irc, int fd);
	int					cmd_server(IRC& irc, int fd);

	std::string const &	getCommand() const;
	bool				check_args_number(int n) const;
	bool				check_password(Client const & client) const;
	bool				check_nickname(Client const & client) const;
};