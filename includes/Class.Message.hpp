
/*
**==========================
** Class Message - Класс реализует логику распарсинга данных по командам:
** PASS - используется для установки 'парольного соединения' 
** NICK - используется для установки пользовательского никнейма или изменения предыдущего
**==========================
*/

#pragma once
#include "main.hpp"
#include "Class.User.hpp"

class Message
{
	private:
		std::vector<std::string>	temp;

		Message(const Message &other);
		Message &operator=(const Message &other);

		bool pass(std::string password);
		bool nick(std::string nickname, std::vector<Client *> users);
		bool user(std::vector<Client *>::iterator it);
		std::vector<Client *>::iterator find_fd(std::vector<Client *> *vect, int fd);
	public:
		Message();
		~Message();

		void	cmd_nick(void * var_1, void * var_2);
		void	cmd_pass(void * var_1, void * var_2);
		void	cmd_user(void * var_1, void * var_2);
		
		void	pars_str(std::string str);

		std::string const &getCommand() const;
};