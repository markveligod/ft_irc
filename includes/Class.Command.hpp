
/*
**==========================
** Class Command - Класс реализует логику распарсинга данных по командам:
** PASS - используется для установки 'парольного соединения' 
** NICK - используется для установки пользовательского никнейма или изменения предыдущего
**==========================
*/

#pragma once
#include "main.hpp"
#include "Class.User.hpp"

class Command
{
private:
	std::string						prefix;
	std::string						command;
	std::vector<std::string> 		arguments;


	bool							nick_length();
	bool							nick_password(Client * cur_client);
	template <typename T> bool		nick_available(std::vector<T> vect);

	bool							pass(std::string password, std::string local_pass);
	bool							user(User *curr_user);
	std::vector<Client *>::iterator find_fd(std::vector<Client *> *vect, int fd);
	
public:
	Command(std::string const & str);
	~Command();

	void				cmd_nick(void * var_1, void * var_2, void * var_3);
	void				cmd_pass(void * var_1, void * var_2, void * var_3);
	void				cmd_user(void * var_1, void * var_2, void * var_3);

	std::string const &	getCommand() const;
};