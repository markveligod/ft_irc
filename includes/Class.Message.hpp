
/*
**==========================
** Class Message - Класс реализует логику распарсинга данных по командам:
** PASS - используется для установки 'парольного соединения' 
** NICK - используется для установки пользовательского никнейма или изменения предыдущего
**==========================
*/

#pragma once
#include "./main.hpp"

class Message
{
    private:
		std::vector<std::string>	temp;

		Message(const Message &other);
        Message &operator=(const Message &other);

        bool pass(std::string password);
        bool nick(std::string nickname, std::vector<User *> users);
		bool user(Client *curr_client);
    public:
        Message();
		~Message();

		void 	cmd_nick(void * var_1, void * var_2);
		void 	cmd_pass(void * var);
		void	cmd_user(void * var);
		
		void	pars_str(std::string str);

		std::string const &getCommand() const;
};