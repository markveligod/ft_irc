#include "Class.Command.hpp"
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

int	Command::cmd_pass(IRC& irc, int fd)
{
	int i;
	bool res;
	std::vector<Client *> &clients	= irc.get_clients();
	std::vector<Server *> &servers 	= irc.get_servers();

	if (!(check_args_number(1)))
		return (ERR_NEEDMOREPARAMS);

	if ((i = IRC::find_fd(clients, fd)) < 0 &&
		(IRC::find_fd(servers, fd)) < 0)
		return (0);								// по идее, такой ситуации быть не может

	if (clients[i]->getPassword() || IRC::find_fd(servers, fd) >= 0)
	{
		Utils::print_error(ERR_ALREADYREGISTRED, "Already registered");
		return (ERR_ALREADYREGISTRED);
	}

	res = this->pass(this->arguments[0], irc.get_localhost_pass());
	clients[i]->setPassword(res);
	return (res == false ? ERR_PASSWDMISMATCH : 0);	// в документации нет такой ошибки
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

int  Command::cmd_nick(IRC& irc, int fd)
{
	std::vector<Client *> &clients 	= irc.get_clients();
	std::vector<User *> &users 		= irc.get_users();
	std::vector<Server *> &servers	= irc.get_servers();
	int i = -1;
	int j = -1;

	if (!(this->check_args_number(1)))
		return (ERR_NONICKNAMEGIVEN);

	if (!(this->nick_length()))									// add check valid!!!
		return (ERR_ERRONEUSNICKNAME);

	if (!(this->nick_available(clients, this->arguments[0])))	// add check for server??? ERR_NICKCOLLISION
		return (ERR_NICKNAMEINUSE);

	if (this->prefix.empty() && (i = IRC::find_fd(clients, fd)) >= 0) 		// если префикс пуст и если есть клиент с таким fd
	{
		if (!(this->check_password(*clients[i])))							// если он уже установил верный пароль
			return 0;
		Utils::print_line("Nickname for client " + this->arguments[0] + " set");
		if ((j = IRC::find_fd(users, fd)) >= 0)
			Utils::print_line("New nickname for user" + this->arguments[0] + " set");
	}

	else if (this->prefix.empty() &&										// если префикс пуст
			IRC::find_fd(clients, fd) < 0 &&								// и нет клиента с таким fd
			(i = IRC::find_fd(servers, fd)) >= 0)							// и есть сервер с таким fd
	{
		Client *new_client = new Client(fd, this->arguments[0], std::atoi(this->arguments[1].c_str()));
		clients.push_back(new_client);
		servers[i]->addClient(new_client);
		Utils::print_line("New client created");
		i = -1;
	}

	else if (!(this->prefix.empty()) && (i = IRC::find_nickname(clients, this->prefix)) >= 0) // если есть префикс и если есть клиент с ником, который пришел в префикс
	{
		if (!(this->check_password(*clients[i])))												// если он уже установил верный пароль
			return 0;
		Utils::print_line("Nickname for client changed from " + clients[i]->getNickname() + " to " + this->arguments[0]);
		if ((j = IRC::find_nickname(users, this->prefix)) >= 0)
			Utils::print_line("Nickname for user changed from " + users[j]->getNickname() + " to " + this->arguments[0]);
	}

	if (i >= 0)
		clients[i]->setNickname(this->arguments[0]);
	if (j >= 0)
		users[j]->setNickname(this->arguments[0]);
	return 0;
}

/*
** =============================================================
** Команда: USER 
** =============================================================
** Параметры: <username> <hostname> <servername> <realname> 
** cmd_user - при удачном вызове дозополняет класс Client
** =============================================================
*/

void Command::user_change(User * curr_user)
{
	curr_user->change_user(this->arguments[0], this->arguments[1],
						   this->arguments[2], this->arguments[3]);
	Utils::print_line("Users information changed");
}

void Command::user_create(Client * curr_client, std::vector<User *> &users, Server * curr_server)
{
	User *curr_user = new User(curr_client);
	curr_user->user_from_client(this->arguments[0], this->arguments[1],
								this->arguments[2], this->arguments[3]);
	users.push_back(curr_user);
	Utils::print_line("USER created");
	if (curr_server != NULL)
	{
		curr_server->addUser(curr_user);
		Utils::print_line("USER added to servers vector");
	}
}

int Command::cmd_user(IRC& irc, int fd)
{
	int i = -1;
	int server_fd = -1;
	std::vector<Client *> &clients	= irc.get_clients();
	std::vector<User *> &users 		= irc.get_users();
	std::vector<Server *> &servers 	= irc.get_servers();

	if (!(this->check_args_number(4)))
		return (ERR_NEEDMOREPARAMS);
	if (this->prefix.empty() &&										// если префикс пуст
	   (i = IRC::find_fd(clients, fd)) >= 0)						// и сообщение от клиента
	{
		if (!(check_password(*clients[i])) ||						// проверяем, ввел ли клиент пароль
			!(check_nickname(*clients[i])))							// и ввел ли клиент ник
			return 0;
		if (IRC::find_fd(users, fd) >= 0)
			return (ERR_ALREADYREGISTRED);
		else
			this->user_create(clients[i], users, NULL);
	}
	else if (!(this->prefix.empty()) &&								// если префикс есть
			(server_fd = IRC::find_fd(servers, fd)) >= 0 &&		// и сообщение пришло с сервера
			(i = IRC::find_nickname(clients, this->prefix)) >= 0)	// и есть клиент с таким ником
	{
		if (!(check_password(*clients[i])) ||						// проверяем, ввел ли клиент пароль
			!(check_nickname(*clients[i])))							// и ввел ли клиент ник
			return 0;
		if (IRC::find_nickname(users, this->prefix))				// если уже есть юзер с таким именем
			return (ERR_ALREADYREGISTRED);
		else
			this->user_create(clients[i], users, servers[server_fd]);
	}
	return 0;
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
	if ((int)this->arguments.size() < n)
	{
		Utils::print_error(ERR_ARG_NUMBER, "Not enought parameters");
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

int Command::cmd_server(IRC& irc, int fd)
{
	std::vector<Client *> vect = irc.get_clients();

	std::vector<Server *> vec_server = irc.get_servers();
	std::vector<Client *>::iterator temp = this->find_fd(&vect, fd);

	if (temp == vect.end())
	{
		Utils::print_error(ERR_FINDFD, "FD don't find in vector!");
		return 0;
	}

	if (!this->check_args_number(3))
		return (ERR_NEEDMOREPARAMS);

	if (!this->check_password(**temp))
		return 0;

	Server *new_server = new Server((*temp)->getSocketFd(), this->arguments[0], atoi(this->arguments[1].c_str()), this->arguments[2]);
	vec_server.push_back(new_server);
	irc.delete_client(fd);
	return 0;
}
