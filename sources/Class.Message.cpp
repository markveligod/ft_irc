#include "../includes/Class.Message.hpp"

/*
**==========================
** конструктор и деструктор Message()
**==========================
*/

Message::Message() {}

Message::~Message() {}


bool Message::pass(std::string password)
{
    if (password == DEF_PASS)
        return true;
    return false;
}

bool Message::nick(std::string nickname)
{
    if (nickname.size() > 9 || nickname.size() == 0)
        return false;
    /*for (std::map<std::string, User>::iterator it = users.begin(); it != users.end(); it++)
    {
        if (nickname == it->second.nickname)
        {
            Utils::print_error(228, "The nickname already exists!");
            return ;
        }
    }*/
    return true;
}

void Message::pars_str(std::string str)
{
    std::istringstream ss(str);
    std::string ff;

    while (getline(ss, ff, ' '))
    {
        //std::cout << ff << std::endl;
        this->temp.push_back(ff);
    }

    if (this->temp[0] == "PASS")
    {
        if (this->pass(this->temp[1]))
            Utils::print_line("Password Correct!");
        else
            Utils::print_error(ERR_PASSWORD, "Password Incorrect!");        
    }
    else if (this->temp[0] == "NICK")
    {
        if (this->nick(this->temp[1]))
        {
            Utils::print_line("nickname is available!");
            this->curr_user.nickname = this->temp[1];
        }
        else
            Utils::print_error(ERR_NICKNAME, "Wrong nickname!");
    }
    this->temp.clear();
}

struct User Message::get_user()
{
    return (this->curr_user);
}
