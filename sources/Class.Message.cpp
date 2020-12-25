#include "../includes/Class.Message.hpp"

/*
**==========================
** конструктор Message(const std::string &str) - парсит команду и вызывает функцию
**==========================
*/

Message::Message(const std::string & str, std::map<std::string, User> &users)
{
    std::vector<std::string> temp;
    std::string tt;
    std::istringstream ss(str);
    
    while (getline(ss, tt, ' '))
    {
        temp.push_back(tt);
    }

    if (temp.size() == 2)
    {
        if (temp[0] == "PASS")
            this->pass(temp[1]);
        else if (temp[1] == "NICK")
            this->nick(temp[1], users);
    }
    else
    {
        Utils::print_error(228, "Ошибка века слишком много текста");
    }
}

void Message::pass(std::string password)
{
    if (password == DEF_PASS)
        Utils::print_line("Password Correct!");
    else
        Utils::print_error(228, "Password Incorrect!");
}

void Message::nick(std::string nickname, std::map<std::string, User> &users)
{
    for (std::map<std::string, User>::iterator it = users.begin(); it != users.end(); it++)
    {
        if (nickname == it->second.nickname)
        {
            Utils::print_error(228, "The nickname already exists!");
            return ;
        }
    }
    Utils::print_line("nickname is available!");
}
