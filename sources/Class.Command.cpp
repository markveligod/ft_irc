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
        return true;
	Utils::print_error(ERR_PASSWORD, "Incorrect password");
	return false;
}

void	Command::cmd_pass(IRC& irc, int fd)
{
	if (args_number(1) &&
		this->pass(this->arguments[0], irc.get_localhost_pass()))
	{
		int i;
		std::vector<Client *> vect = irc.get_clients();
		
		if ((i = IRC::find_fd(&vect, fd)) < 0)
		{
			Utils::print_error(ERR_FINDFD, "FD don't find in vector!");
			return ;
		}
		vect[i]->setPassword();
		std::cout << "Correct password\n";
	}
	this->arguments.clear();
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

bool Command::nick_password(Client * cur_client)
{
	if (cur_client->getPassword() == false)
	{
		Utils::print_error(123, "Log in with correct PASS at first");
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
	std::vector<Client *> clients = irc.get_clients();
	std::vector<User *> users = irc.get_users();
	int i = 0;

	//
	// если клиент подключен к локальному серверу
	//
	if (!(this->nick_length()) ||
		!(this->nick_available(clients, this->arguments[0])))
		return;
	if ((i = IRC::find_fd(&clients, fd)) >= 0)
	{
		Client *cur_client = clients[i];
		if (this->nick_password(cur_client))
		{
			if (cur_client->getNickname().empty())
				Utils::print_line("New nickname for client " + this->arguments[0] + " set");
			else
				Utils::print_line("Nickname for client changed from " + cur_client->getNickname() + " to " + this->arguments[0]);
			cur_client->setNickname(this->arguments[0]);

			if ((i = IRC::find_fd(&users, fd)) >= 0 &&
				this->nick_available(users, this->arguments[0]))
			{
				User *cur_user = users[i];
				if (cur_user->getNickname().empty())
					Utils::print_line("New nickname for user" + this->arguments[0] + " set");
				else
					Utils::print_line("Nickname for user changed from " + cur_user->getNickname() + " to " + this->arguments[0]);
				cur_user->setNickname(this->arguments[0]);
			}
		}
	}
	//
	// если клиент подключен через другой сервер
	else
	// добавить - если дексриптор есть среди дексрипторов серверов
	{
		if (this->prefix.empty())
		{
			Client *new_client = new Client(fd, this->arguments[0], std::stoi(this->arguments[1]));
			clients.push_back(new_client);
		}
		else if (!(this->nick_available(clients, this->prefix)) &&
				 (i = IRC::find_nickname(&clients, this->arguments[0])) >= 0)
		{
			Client *cur_client = clients[i];
			if (this->nick_password(cur_client))
			{
				cur_client->setNickname(this->arguments[0]);
				Utils::print_line("Nickname for client changed from " + cur_client->getNickname() + " to " + this->arguments[0]);
			}
			if (!(this->nick_available(users, this->prefix)) && (i = IRC::find_nickname(&users, this->prefix)) >= 0)
			{
				User *cur_user = users[i];
				cur_user->setNickname(this->arguments[0]);
				Utils::print_line("Nickname for user changed from " + cur_user->getNickname() + " to " + this->arguments[0]);
			}
		}
	}
	// получается, если нам пришел запрос на изменение ника пользователя с другого сервера,
	// мы должны не только у сея изменить эти данные, но и отправить эту же команду
	// другим серверам ? (а что, если им тот сервер, что нам отправил запрос, уже отправил его и им, и мы отправил повторно???)
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

	std::vector<User *> vec_user =irc.get_users();
	std::vector<Client *>::iterator temp;

	if ((temp = this->find_fd(&vect, fd)) == vect.end())
	{
		Utils::print_error(ERR_FINDFD, "FD don't find in vector!");
		return ;
	}

	if (!(*temp)->getPassword())
	{
		Utils::print_error(ERR_PASSWORD, "Enter PASS <password>");
		return ;
	}

	if ((*temp)->getNickname().size() == 0)
	{
		Utils::print_error(ERR_NICKNAME, "ENTER NICK <nickname>");
		return ;
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

bool Command::args_number(int n) const
{
	if ((int)this->arguments.size() != n)
	{
		Utils::print_error(ERR_ARG_NUMBER, "Invalid number of arguments");
		return false;
	}
	return true;
}
