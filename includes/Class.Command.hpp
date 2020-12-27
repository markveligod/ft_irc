
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
		std::vector<std::string>	temp;

		Command(const Command &other);
		Command &operator=(const Command &other);

		bool pass(std::string password, std::string local_pass);
		bool nick(std::string nickname, std::vector<Client *> users);
		bool user(User *curr_user);
		std::vector<Client *>::iterator find_fd(std::vector<Client *> *vect, int fd);
	public:
		Command();
		~Command();

		void	cmd_nick(void * var_1, void * var_2, void *var_3);
		void	cmd_pass(void * var_1, void * var_2, void *var_3);
		void	cmd_user(void *var_1, void *var_2, void *var_3);
		
		void	pars_str(std::string str);

		std::string const &getCommand() const;
};