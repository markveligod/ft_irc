#include "../includes/Class.Message.hpp"

/*
**==========================
** конструктор и деструктор Message()
**==========================
*/

Message::Message() {}

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

bool Message::nick(std::string nickname, std::vector<Client *> users)
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

bool Message::user(std::vector<Client *>::iterator it)
{
	std::string temp_str;
	if (this->temp.size() < 4)
		return false;
    (*it)->setUsername(this->temp[1]);
	(*it)->setHostname(this->temp[2]);
	(*it)->setServername(this->temp[3]);
	for (size_t i = 4; i < this->temp.size(); i++)
		temp_str += this->temp[i];
	(*it)->setRealname(temp_str);
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
** find_fd - находит в векторе итераторную позицию которая соответствует переданный fd
**==========================
*/

std::vector<Client *>::iterator Message::find_fd(std::vector<Client *> *vect, int fd)
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

void  Message::cmd_nick(void * var_1, void * var_2)
{
	int *fd 								= (int *)var_2;
	std::vector<Client *> *vect				= (std::vector<Client *> *)var_1;
	std::vector<Client *>::iterator temp;

	if ((temp = this->find_fd(vect, *fd)) == (*vect).end())
	{
		Utils::print_error(ERR_FINDFD, "FD don't find in vector!");
		return ;
	}
	if ((*temp)->getPassword() == false)
		Utils::print_error(123, "Enter PASS before NICK!");
	else if (this->nick(this->temp[1], *vect))
	{
		Utils::print_line("NickName is avalible!");
		(*temp)->setNickname(this->temp[1]);
	}
	else
		Utils::print_error(ERR_NICKNAME, "NickName is not avalible!");
	
	this->temp.clear();
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

void	Message::cmd_pass(void * var_1, void * var_2)
{
	if (this->pass(this->temp[1]))
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
	this->temp.clear();
}

/*
**==========================
** cmd_user - при удачном вызове дозополняет класс Client
**==========================
*/

void Message::cmd_user(void *var_1, void *var_2, void *var_3)
{
	std::vector<Client *> *vect = (std::vector<Client *> *)var_1;

	int *fd = (int *)var_2;
	User *curr_user = (User *)var_3;
	std::vector<Client *>::iterator temp;

	if ((temp = this->find_fd(vect, *fd)) == (*vect).end())
	{
		Utils::print_error(ERR_FINDFD, "FD don't find in vector!");
		return ;
	}

	if ((*temp)->getPassword() && (*temp)->getNickname() == curr_user->getNickname())
	{
		if (this->user(curr_user))
		{
			Utils::print_line("Client is done!");
		}
		else
			Utils::print_error(ERR_DATACLIENT, "Incorrect client data!");
	}
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
