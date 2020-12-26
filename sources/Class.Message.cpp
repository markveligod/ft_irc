#include "../includes/Class.Message.hpp"

/*
**==========================
** конструктор и деструктор Message()
**==========================
*/

Message::Message() {}

//Message::Message(std::vector<User *> all_users) : all_users(all_users) {}

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

    /*if (this->temp[0] == "PASS")
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
            //this->curr_user.nickname = this->temp[1];
        }
        else
            Utils::print_error(ERR_NICKNAME, "Wrong nickname!");
    }
    this->temp.clear();*/
}

void  Message::cmd_nick(void * var)
{
	//User *new_user = new User(this->temp[1]);
	std::vector<User *> *vect = (std::vector<User *> *)var;
	(*vect)[0]->client_from_pass(this->temp[1]);
	this->temp.clear();
	//vect->push_back(new_user);
}

void	Message::cmd_pass(void * var)
{
	if (this->temp[1] == DEF_PASS)
	{
		User *new_user = new User();
		std::vector<User *> *vect = (std::vector<User *> *)var;
		vect->push_back(new_user);
	}
	this->temp.clear();
	/*if (this->temp[1] == DEF_PASS)
		return ((void *)true);
	return ((void *)false);*/
}

//typedef void *(Message::*doCommand)(void);

/*void * Message::do_cmd()
{
	std::string cmd_name[2] = {"NICK",
							   "PASS"};
	doCommand cmd_func[2] 	= {&Message::cmd_nick,
							   &Message::cmd_pass};
	void *		attribute[2] = 
	for (int i = 0; i < 2; i++)
	{
		if (cmd_name[i] == this->temp[0])
			return ((this->*(cmd_func[i]))());
	}
	return (NULL);
}*/

std::string const & Message::getCommand() const
{
	return (this->temp[0]);
}

/*struct User Message::get_user()
{
    return (this->curr_user);
}*/
