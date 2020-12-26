
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

//temp
//struct User {
  //  std::string nickname;
//};

class Message
{
    private:
        //struct User curr_user;
		//User						curr_user;
		std::vector<std::string>	temp;
		//std::vector<User *>			all_users;

		Message(const Message &other);
        Message &operator=(const Message &other);

        bool pass(std::string password);
        bool nick(std::string nickname);
    public:
        Message();
		//Message(std::vector<User *> all_users);
		~Message();

		void 	cmd_nick(void * var);
		void 	cmd_pass(void * var);
		//void *	do_cmd();
		void	pars_str(std::string str);
		//struct User get_user();

		std::string const &getCommand() const;
};