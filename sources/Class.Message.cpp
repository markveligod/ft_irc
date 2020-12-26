#include "../includes/Class.Message.hpp"

/*
**==========================
** конструктор и деструктор Message()
**==========================
*/

Message::Message() {}

//Message::Message(std::vector<User *> all_users) : all_users(all_users) {}

Message::~Message() {}

/*
**==========================
** Команда: PASS 
** Параметры: <password>
**==========================
*/

bool Message::pass(std::string password)
{
    if (password == DEF_PASS)
        return true;
    return false;
}

/*
**==========================
** Команда: NICK 
** Параметры: <nickname> [ <hopcount> ]
**==========================
*/

bool Message::nick(std::string nickname, std::vector<User *> users)
{
    if (nickname.size() > 9 || nickname.size() == 0)
        return false;
    for (size_t i = 0; i < users.size(); i++)
	{
		if (users[i]->getNickname() == nickname)
			return false;
	}
    return true;
}

/*
**==========================
** Команда: USER 
** Параметры: <username> <hostname> <servername> <realname> 
**==========================
*/

bool Message::user(Client *curr_client)
{
	if (this->temp.size() < 4)
		return false;
    curr_client->username = this->temp[1];
	curr_client->hostname = this->temp[2];
	curr_client->servername = this->temp[3];
	for (size_t i = 4; i < this->temp.size(); i++)
		curr_client->realname += this->temp[i];
	return true;
}

/*
**==========================
** pars_str - парсинг строки из буфера 
**==========================
*/

void Message::pars_str(std::string str)
{
    std::istringstream ss(str);
    std::string ff;

    while (getline(ss, ff, ' '))
    {
        //std::cout << ff << std::endl;
        this->temp.push_back(ff);
    }
}

/*
**==========================
** cmd_nick - при удачном вызове добавляет никнэйм в класс Client
**==========================
*/

void  Message::cmd_nick(void * var_1, void * var_2)
{
	std::vector<User *> *vect = (std::vector<User *> *)var_1;
	Client *curr_client = (Client *)var_2;

	if (this->nick(this->temp[1], *vect))
	{
		Utils::print_line("NickName is avalible!");
		curr_client->SetNickname = this->temp[1];
	}
	else
		Utils::print_error(ERR_NICKNAME,"NickName is not avalible!");
	this->temp.clear();
}

/*
**==========================
** cmd_pass - при удачном вызове меняет статус класса Client на корректный ввод пароля
**==========================
*/

void	Message::cmd_pass(void * var)
{
	Client *curr_client = (Client *)var;
	if (this->pass(this->temp[1]))
	{
		Utils::print_line("Correct password!");
		curr_client->SetPassStatus = true;
	}
	else
		Utils::print_error(ERR_PASSWORD, "Incorrect password");
	this->temp.clear();
}

/*
**==========================
** cmd_user - при удачном вызове дозополняет класс Client
**==========================
*/

void Message::cmd_user(void * var)
{
	Client *curr_client = (Client *)var;
	if (this->user(curr_client))
		Utils::print_line("Client is done!");
	else
		Utils::print_error(ERR_DATACLIENT, "Incorrect client data!");

}

/*
**==========================
** getCommand - возвращает текущую команду из буфера
**==========================
*/

std::string const & Message::getCommand() const
{
	return (this->temp[0]);
}
