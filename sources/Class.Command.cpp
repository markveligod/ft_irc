#include "../includes/Class.Command.hpp"
#include "Class.IRC.hpp"

/*
** ==========================================
** конструктор и деструктор Command()
** ==========================================
*/

Command::Command(std::string const & str)
{
	std::istringstream	ss(str);
	std::string			word;
	std::string			last_arg;

	getline(ss, word, ' ');
	if (word[0] == ':')
	{
		this->prefix = word.substr(1, word.length());
		getline(ss, this->command, ' ');
	}
	else
		this->command = word;

	while (getline(ss, word, ' '))
	{
		if (word[0] == ':')
		{
			last_arg = word.substr(1, word.length());
			break;
		}
		this->arguments.push_back(word);
	}
	while (getline(ss, word, ' '))
		last_arg = last_arg + ' ' + word;
	if (!(last_arg.empty()))
	this->arguments.push_back(last_arg);
}

Command::~Command() {}

/*
** ================================================================
** Команда: PASS
** ================================================================
** cmd_pass		-	при удачном вызове меняет статус 
**					класса Client на корректный ввод пароля
** принимает	-	var_1 - указатель на вектор клиентов
**					var_2 - файловый дескриптор сокета, по которому
**							пришел запрос на выполнение команды
** суть работы	-	если пароль соответствует паролю сервера,
**					ищем по вектору всех подключенных клиентов,
**					(при помощи fd сокета),
**					от кого пришел запрос на вход с таким паролем,
**					и этому клиенту меняем статус пароля на true
** =================================================================
*/

bool Command::pass(std::string password, std::string local_pass)
{
    if (password == local_pass)
	{
		Utils::print_line("Correct password");
		return true;
	}
	Utils::print_error(ERR_PASSWORD, "Incorrect password set");
	return false;
}

void	Command::cmd_pass(IRC& irc, int fd)
{
	int i;
	int j;
	bool res;
	std::vector<Client *> clients	= irc.get_clients();
	std::vector<User *> users 		= irc.get_users();

	if (!(check_args_number(1)))
		return;
	if (this->prefix.empty())
	{
		if ((i = IRC::find_fd(&clients, fd)) < 0)
		{
			Utils::print_error(ERR_FINDFD, "There is no client with such fd");
			return;
		}
		j = IRC::find_fd(&users, fd);
	}
	else
	{
		if ((i = IRC::find_nickname(&clients, this->prefix)) < 0)
		{
			Utils::print_error(ERR_FINDFD, "There is no client with such nickname");
			return;
		}
		j = IRC::find_nickname(&users, this->prefix);
	}
	res = this->pass(this->arguments[0], irc.get_localhost_pass());
	clients[i]->setPassword(res);
	if (j >= 0)
		users[j]->setPassword(res);
}

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

bool Command::nick_length()
{
	if (this->arguments[0].size() > 9 || this->arguments[0].size() == 0)
	{
		Utils::print_error(ERR_NICKNAME, "Nickname length must be between 1 and 9 symbols");
		return false;
	}
	return true;
}

template <typename T>
bool Command::nick_available(std::vector<T> vect, std::string const &nick)
{
	for (size_t i = 0; i < vect.size(); i++)
	{
		if (vect[i]->getNickname() == nick)
		{
			Utils::print_error(ERR_NICKNAME, "Nickname is already in use");
			return false;
		}
	}
	return true;
}

void  Command::cmd_nick(IRC& irc, int fd)
{
	std::vector<Client *> clients 	= irc.get_clients();
	std::vector<User *> users 		= irc.get_users();
	std::vector<Server *> servers	= irc.get_servers();
	int i = -1;
	int j = -1;

	if (!(this->nick_length()) ||
		!(this->nick_available(clients, this->arguments[0])))
		return;

	if (this->prefix.empty() && (i = IRC::find_fd(&clients, fd)) >= 0) // если префикс пуст и если есть клиент с таким fd
	{
		if (!(this->check_password(*clients[i])))							// если он уже установил верный пароль
			return;
		Utils::print_line("Nickname for client " + this->arguments[0] + " set");
		if ((j = IRC::find_fd(&users, fd)) >= 0)
			Utils::print_line("New nickname for user" + this->arguments[0] + " set");
	}

	else if (this->prefix.empty() && IRC::find_fd(&clients, fd) < 0) // если префикс пуст и если нет клиента с таким fd //!!!!!&& (IRC::find_fd(&servers, fd)) >= 0)!!!!!
		clients.push_back(new Client(fd, this->arguments[0], std::atoi(this->arguments[1].c_str())));

	else if (!(this->prefix.empty()) && (i = IRC::find_nickname(&clients, this->prefix)) >= 0) // если есть префикс и если есть клиент с ником, который пришел в префикс
	{
		if (!(this->check_password(*clients[i])))												// если он уже установил верный пароль
			return;
		Utils::print_line("Nickname for client changed from " + clients[i]->getNickname() + " to " + this->arguments[0]);
		if ((j = IRC::find_nickname(&users, this->prefix)) >= 0)
			Utils::print_line("Nickname for user changed from " + users[j]->getNickname() + " to " + this->arguments[0]);
	}

	if (i >= 0)
		clients[i]->setNickname(this->arguments[0]);
	if (j >= 0)
		users[j]->setNickname(this->arguments[0]);
}

/*
** =============================================================
** Команда: USER 
** =============================================================
** Параметры: <username> <hostname> <servername> <realname> 
** cmd_user - при удачном вызове дозополняет класс Client
** =============================================================
*/

bool Command::user(User *curr_user)
{
	std::string temp_str;
	if (this->arguments.size() != 4)
		return false;
	curr_user->user_from_client(this->arguments[0], this->arguments[1], this->arguments[2], this->arguments[3]);
	return true;
}

void Command::cmd_user(IRC& irc, int fd)
{
	std::vector<Client *> vect = irc.get_clients();

	std::vector<User *> vec_user = irc.get_users();
	std::vector<Client *>::iterator temp;

	if ((temp = this->find_fd(&vect, fd)) == vect.end())
	{
		Utils::print_error(ERR_FINDFD, "FD don't find in vector!");
		return;
	}

	if (!(*temp)->getPassword())
	{
		Utils::print_error(ERR_PASSWORD, "Enter PASS <password>");
		return;
	}

	if ((*temp)->getNickname().size() == 0)
	{
		Utils::print_error(ERR_NICKNAME, "ENTER NICK <nickname>");
		return;
	}

	User *curr_user = new User(*temp);

	if (this->user(curr_user))
	{
		vec_user.push_back(curr_user);
		Utils::print_line("Client is done!");
	}
	else
	{
		delete curr_user;
		Utils::print_error(ERR_DATACLIENT, "Incorrect client data!");
	}

	/*int i;
	std::vector<Client *> clients 	= irc.get_clients();
	std::vector<User *> users 		= irc.get_users();

	if ()
	if ((i = IRC::find_fd(&clients, fd)) >= 0)
	{
		Utils::print_error(ERR_FINDFD, "FD don't find in vector!");
		return ;
	}

	if (!(check_password(*clients[i])) ||
		!(check_nickname(*clients[i])))
		return ;

	User *curr_user = new User(*temp);

	if (this->user(curr_user))
	{
		vec_user.push_back(curr_user);
		Utils::print_line("Client is done!");
	}
	else
	{
		delete curr_user;
		Utils::print_error(ERR_DATACLIENT, "Incorrect client data!");
	}*/
}

/*
** =====================================================================
** find_fd - находит в векторе итераторную позицию которая соответствует 
**           переданный fd
** =====================================================================
*/

std::vector<Client *>::iterator Command::find_fd(std::vector<Client *> *vect, int fd)
{
	std::vector<Client *>::iterator v_begin = (*vect).begin();
	std::vector<Client *>::iterator v_end	= (*vect).end();

	while (v_begin != v_end)
	{
		if ((*v_begin)->getSocketFd() == fd)
			return (v_begin);
		v_begin++;
	}
	return (v_end);
}

/*
** ==================================================
** getCommand - возвращает текущую команду из буфера
** ==================================================
*/

std::string const & Command::getCommand() const
{
	return (this->command);
}

bool Command::check_args_number(int n) const
{
	if ((int)this->arguments.size() != n)
	{
		Utils::print_error(ERR_ARG_NUMBER, "Invalid number of arguments");
		return false;
	}
	return true;
}

bool Command::check_password(Client const &client) const
{
	if (client.getPassword() == false)
	{
		Utils::print_error(123, "ENTER PASS <password>");
		return false;
	}
	return true;
}

bool Command::check_nickname(Client const & client) const
{
	if (client.getNickname().empty())
	{
		Utils::print_error(ERR_NICKNAME, "ENTER NICK <nickname>");
		return false;
	}
	return true;
}

/*
** ==================================================
**	cmd_server - добавляет в вектор серверов => текущий сервер
**	Команда: SERVER
**	Параметры: <servername> <hopcount> <info>
** ==================================================
*/

void Command::cmd_server(IRC& irc, int fd)
{
	std::vector<Client *> vect = irc.get_clients();

	std::vector<Server *> vec_server = irc.get_servers();
	std::vector<Client *>::iterator temp = this->find_fd(&vect, fd);

	if (temp == vect.end())
	{
		Utils::print_error(ERR_FINDFD, "FD don't find in vector!");
		return ;
	}

	if (this->arguments.size() > 3)
	{
		Utils::print_error(ERR_ARG_NUMBER, "Invalid number of arguments!");
		return ;
	}

	if ((*temp)->getPassword())
	{
		Server *new_server = new Server((*temp)->getSocketFd(), this->arguments[0], atoi(this->arguments[1].c_str()), this->arguments[2]);
		vec_server.push_back(new_server);
	}
	else
		Utils::print_error(ERR_PASSWORD, "Enter PASS <password>!");
}
