
/*
**==========================
** Class Message - Класс реализует логику распарсинга данных по командам:
** PASS - используется для установки 'парольного соединения' 
** NICK - используется для установки пользовательского никнейма или изменения предыдущего
**==========================
*/

#pragma once
#include "./main.hpp"

//temp
struct User {
    std::string nickname;
};

class Message
{
    private:
        struct User curr_user;
        std::vector<std::string> temp;

        Message(const Message &other);
        Message &operator=(const Message &other);

        bool pass(std::string password);
        bool nick(std::string nickname);
    public:
        Message();
        ~Message();

        void pars_str(std::string str);
        struct User get_user();
        
};