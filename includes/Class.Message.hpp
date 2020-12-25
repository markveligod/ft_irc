
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
        Message();
        Message(const Message &other);
        Message &operator=(const Message &other);
        ~Message();

        void pass(std::string password);
        void nick(std::string nickname, std::map<std::string, User> &users);
    public:
        Message(const std::string & str, std::map<std::string, User> &users);
        
};