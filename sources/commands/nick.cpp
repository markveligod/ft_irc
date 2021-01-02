#include "Class.Command.hpp"
#include "Class.IRC.hpp"

/*
** ====================================================================
** Команда: NICK 
** ====================================================================
** Параметры: <nickname> [ <hopcount> ]
** cmd_nick		-	при удачном вызове добавляет никнэйм в класс Client
**					
** принимает	-	var_1 - указатель на вектор клиентов
**					var_2 - файловый дескриптор сокета, по которому
**							пришел запрос на выполнение команды
** суть работы	-	ищем по вектору всех подключенных клиентов,
**					от кого пришел запрос создание nickname 
**					(при помощи fd сокета),
**					проверяем, вошел ли этот клиент по паролю,
**					если да, то выставляем ему nickname
** =====================================================================
*/

bool Command::
nick_valid() const
{
	int i = 1;
	if (this->arguments[0].size() > 9 || this->arguments[0].size() == 0)
	{
		utils::print_error(ERR_NICKNAME, "Nickname length must be between 1 and 9 symbols");
		return false;
	}
	if (!(std::isalpha(this->arguments[0][0])))
	{
		utils::print_error(ERR_NICKNAME, "First symbol must be a letter");
		return false;
	}
	while (i != (int)this->arguments[0].size())
	{
		if (std::isalnum(this->arguments[0][i]) ||
			std::strchr("-[]\\`^{}", this->arguments[0][i]) != NULL)
			i++;
		else
		{
			utils::print_error(ERR_NICKNAME, "Not valid symbols");
			return false;
		}
	}
	return true;
}

template <typename T>
bool Command::
nick_available(std::vector<T> vect, std::string const &nick)
{
	for (size_t i = 0; i < vect.size(); i++)
	{
		if (vect[i]->getNickname() == nick)
		{
			utils::print_error(ERR_NICKNAME, "Nickname is already in use");
			return false;
		}
	}
	return true;
}

int Command::
cmd_nick(IRC& irc, int fd)
{
	std::vector<Client *> &clients 	= irc.get_clients();
	std::vector<User *> &users 		= irc.get_users();
	std::vector<Server *> &servers	= irc.get_servers();
	int i = -1;
	int j = -1;

	if (!(this->check_args_number(1)))
		return (ERR_NONICKNAMEGIVEN);

	if (!(this->nick_valid()))
		return (ERR_ERRONEUSNICKNAME);

	if (IRC::find_fd(servers, fd) >= 0 &&
		this->prefix.empty() &&									
		!(this->nick_available(clients, this->arguments[0])))				// ????????
		return (ERR_NICKCOLLISION);

	if (!(this->nick_available(clients, this->arguments[0])))
		return (ERR_NICKNAMEINUSE);

	if (this->prefix.empty() && (i = IRC::find_fd(clients, fd)) >= 0) 		// если префикс пуст и если есть клиент с таким fd
	{
		if (!(this->check_password(*clients[i])))							// если он уже установил верный пароль
			return 0;
		utils::print_line("Nickname for client " + this->arguments[0] + " set");
		if ((j = IRC::find_fd(users, fd)) >= 0)
			utils::print_line("New nickname for user" + this->arguments[0] + " set");
	}

	else if (this->prefix.empty() &&										// если префикс пуст
			IRC::find_fd(clients, fd) < 0 &&								// и нет клиента с таким fd
			(i = IRC::find_fd(servers, fd)) >= 0)							// и есть сервер с таким fd
	{
		Client *new_client = new Client(fd, this->arguments[0], std::atoi(this->arguments[1].c_str()));
		clients.push_back(new_client);
		servers[i]->addClient(new_client);
		utils::print_line("New client created");
		i = -1;
	}

	else if (!(this->prefix.empty()) && (i = IRC::find_nickname(clients, this->prefix)) >= 0) // если есть префикс и если есть клиент с ником, который пришел в префикс
	{
		if (!(this->check_password(*clients[i])))												// если он уже установил верный пароль
			return 0;
		utils::print_line("Nickname for client changed from " + clients[i]->getNickname() + " to " + this->arguments[0]);
		if ((j = IRC::find_nickname(users, this->prefix)) >= 0)
			utils::print_line("Nickname for user changed from " + users[j]->getNickname() + " to " + this->arguments[0]);
	}

	if (i >= 0)
		clients[i]->setNickname(this->arguments[0]);
	if (j >= 0)
		users[j]->setNickname(this->arguments[0]);
	return 0;
}
