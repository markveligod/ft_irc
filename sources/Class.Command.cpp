#include "../includes/Class.Command.hpp"

/*
**==========================
** конструктор и деструктор Command()
**==========================
*/

Command::Command(std::string const & str)
{
	std::istringstream	ss(str);
	std::string			word;
	std::string			last_arg;

	getline(ss, word, ' ');
	if (word[0] == ':')
		this->prefix = word.substr(1, word.length());
	else
		this->command = word;

	if (this->command.empty())
		getline(ss, this->command, ' ');

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
	this->arguments.push_back(last_arg);
}

Command::~Command() {}

/*
**==========================
** Команда: PASS 
** Параметры: <password>
**==========================
*/

bool Command::pass(std::string password, std::string local_pass)
{
    if (password == local_pass)
        return true;
    return false;
}

/*
**==========================
** Команда: NICK 
** Параметры: <nickname> [ <hopcount> ]
**==========================
*/

bool Command::nick(std::string nickname, std::vector<Client *> users)
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

bool Command::user(User *curr_user)
{
	std::string temp_str;
	if (this->arguments.size() != 4)
		return false;
	curr_user->user_from_client(this->arguments[0], this->arguments[1], this->arguments[2], this->arguments[4]);
	return true;
}

/*
**==========================
** find_fd - находит в векторе итераторную позицию которая соответствует переданный fd
**==========================
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
** ================================================================
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
** =================================================================
*/

void  Command::cmd_nick(void * var_1, void * var_2, void * var_3)
{
	int *fd 								= (int *)var_2;
	std::vector<Client *> *vect				= (std::vector<Client *> *)var_1;
	std::vector<Client *>::iterator temp;

	(void)var_3;
	if ((temp = this->find_fd(vect, *fd)) == (*vect).end())
	{
		Utils::print_error(ERR_FINDFD, "FD don't find in vector!");
		return ;
	}
	if ((*temp)->getPassword() == false)
		Utils::print_error(123, "Enter PASS before NICK!");
	else if (this->nick(this->arguments[0], *vect))
	{
		Utils::print_line("NickName is avalible!");
		(*temp)->setNickname(this->arguments[0]);
	}
	else
		Utils::print_error(ERR_NICKNAME, "NickName is not avalible!");
	
	this->arguments.clear();
}

/*
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

void	Command::cmd_pass(void * var_1, void * var_2, void * var_3)
{
	std::string *local_pass = (std::string *)var_3;
	if (this->pass(this->arguments[0], *local_pass))
	{
		int *fd 								= (int *)var_2;
		std::vector<Client *> *vect 			= (std::vector<Client *> *)var_1;
		std::vector<Client *>::iterator temp;
		
		if ((temp = this->find_fd(vect, *fd)) == (*vect).end())
		{
			Utils::print_error(ERR_FINDFD, "FD don't find in vector!");
			return ;
		}

		(*temp)->setPassword();
		std::cout << "Correct password\n";
	}
	else
		Utils::print_error(ERR_PASSWORD, "Incorrect password");
	this->arguments.clear();
}

/*
**==========================
** cmd_user - при удачном вызове дозополняет класс Client
**==========================
*/

void Command::cmd_user(void *var_1, void *var_2, void *var_3)
{
	std::vector<Client *> *vect = (std::vector<Client *> *)var_1;

	int *fd = (int *)var_2;
	std::vector<User *> *vec_user = (std::vector<User *> *)var_3;
	std::vector<Client *>::iterator temp;

	if ((temp = this->find_fd(vect, *fd)) == (*vect).end())
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

	User *curr_user = new User(**temp);

	if (this->user(curr_user))
	{
		(*vec_user).push_back(curr_user);
		Utils::print_line("Client is done!");
	}
	else
	{
		delete curr_user;
		Utils::print_error(ERR_DATACLIENT, "Incorrect client data!");
	}
}

/*
**==========================
** getCommand - возвращает текущую команду из буфера
**==========================
*/

std::string const & Command::getCommand() const
{
	return (this->command);
}
