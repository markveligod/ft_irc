#include "Class.Command.hpp"
#include "Class.IRC.hpp"

using std::vector;
using std::string;

/*
** ==========================================
** конструктор и деструктор Command()
** ==========================================
*/

Command::Command(const string& str)
{
	std::istringstream	ss(str);
	string				word;
	string				last_arg;

	getline(ss, word, ' ');
	if (word[0] == ':')
	{
		this->prefix = word.substr(1, word.length());
		getline(ss, this->command, ' ');
	}
	else
	{
		this->prefix = string();
		this->command = word;
	}

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

bool Command::
pass(string password, string local_pass)
{
    if (password == local_pass)
	{
		utils::print_line("Correct password");
		return true;
	}
	utils::print_error(ERR_PASSWORD, "Incorrect password set");
	return false;
}

int	Command::
cmd_pass(IRC& irc, int fd)
{
	int i;
	bool res;
	vector<Client*>& clients	= irc.get_clients();
	vector<Server*>& servers 	= irc.get_servers();

	if (!(check_args_number(1)))
		return (ERR_NEEDMOREPARAMS);

	if ((i = IRC::find_fd(clients, fd)) < 0 &&
		(IRC::find_fd(servers, fd)) < 0)
		return (0);								// по идее, такой ситуации быть не может

	if (clients[i]->getPassword() || IRC::find_fd(servers, fd) >= 0)
	{
		utils::print_error(ERR_ALREADYREGISTRED, "Already registered");
		return (ERR_ALREADYREGISTRED);
	}

	res = this->pass(this->arguments[0], irc.get_localhost_pass());
	clients[i]->setPassword(res);
	return (res == false ? ERR_PASSWDMISMATCH : 0);	// в документации нет такой ошибки
}

// int  Command::cmd_nick(IRC& irc, int fd)
// {
// 	vector<Client*>& clients 	= irc.get_clients();
// 	vector<User*>& users 		= irc.get_users();
// 	vector<Server*>& servers	= irc.get_servers();
// 	int i = -1;
// 	int j = -1;

// 	if (!(this->check_args_number(1)))
// 		return (ERR_NONICKNAMEGIVEN);

// 	if (!(this->nick_valid()))
// 		return (ERR_ERRONEUSNICKNAME);

// 	if (IRC::find_fd(servers, fd) >= 0 &&
// 		this->prefix.empty() &&									
// 		!(this->nick_available(clients, this->arguments[0])))				// ????????
// 		return (ERR_NICKCOLLISION);

// 	if (!(this->nick_available(clients, this->arguments[0])))
// 		return (ERR_NICKNAMEINUSE);

// 	if (this->prefix.empty() && (i = IRC::find_fd(clients, fd)) >= 0) 		// если префикс пуст и если есть клиент с таким fd
// 	{
// 		if (!(this->check_password(*clients[i])))							// если он уже установил верный пароль
// 			return 0;
// 		utils::print_line("Nickname for client " + this->arguments[0] + " set");
// 		if ((j = IRC::find_fd(users, fd)) >= 0)
// 			utils::print_line("New nickname for user" + this->arguments[0] + " set");
// 	}

// 	else if (this->prefix.empty() &&										// если префикс пуст
// 			IRC::find_fd(clients, fd) < 0 &&								// и нет клиента с таким fd
// 			(i = IRC::find_fd(servers, fd)) >= 0)							// и есть сервер с таким fd
// 	{
// 		Client* new_client = new Client(fd, this->arguments[0], std::atoi(this->arguments[1].c_str()));
// 		clients.push_back(new_client);
// 		servers[i]->addClient(new_client);
// 		utils::print_line("New client created");
// 		i = -1;
// 	}

// 	else if (!(this->prefix.empty()) && (i = IRC::find_nickname(clients, this->prefix)) >= 0) // если есть префикс и если есть клиент с ником, который пришел в префикс
// 	{
// 		if (!(this->check_password(*clients[i])))												// если он уже установил верный пароль
// 			return 0;
// 		utils::print_line("Nickname for client changed from " + clients[i]->getNickname() + " to " + this->arguments[0]);
// 		if ((j = IRC::find_nickname(users, this->prefix)) >= 0)
// 			utils::print_line("Nickname for user changed from " + users[j]->getNickname() + " to " + this->arguments[0]);
// 	}

// 	if (i >= 0)
// 		clients[i]->setNickname(this->arguments[0]);
// 	if (j >= 0)
// 		users[j]->setNickname(this->arguments[0]);
// 	return 0;
// }

/*
** =============================================================
** Команда: USER 
** =============================================================
** Параметры: <username> <hostname> <servername> <realname> 
** cmd_user - при удачном вызове дозополняет класс Client
** =============================================================
*/

void Command::
user_change(User* curr_user)
{
	curr_user->change_user(this->arguments[0], this->arguments[1],
						   this->arguments[2], this->arguments[3]);
	utils::print_line("Users information changed");
}

void Command::
user_create(Client* curr_client, vector<User*>& users, Server* curr_server)
{
	User* curr_user = new User(curr_client);
	curr_user->user_from_client(this->arguments[0], this->arguments[1],
								this->arguments[2], this->arguments[3]);
	users.push_back(curr_user);
	utils::print_line("USER created");

	if (curr_server != NULL)
	{
		curr_server->addUser(curr_user);
		utils::print_line("USER added to servers vector");
	}
}

int Command::
cmd_user(IRC& irc, int fd)
{
	int i = -1;
	int server_fd = -1;
	vector<Client*>& clients	= irc.get_clients();
	vector<User*>& users 		= irc.get_users();
	vector<Server*>& servers 	= irc.get_servers();

	if (!(this->check_args_number(4)))
		return (ERR_NEEDMOREPARAMS);
	if (this->prefix.empty() &&										// если префикс пуст
	   (i = IRC::find_fd(clients, fd)) >= 0)						// и сообщение от клиента
	{
		if (!(check_password(*clients[i])) ||						// проверяем, ввел ли клиент пароль
			!(check_nickname(*clients[i])))							// и ввел ли клиент ник
			return 0;
		if (IRC::find_fd(users, fd) >= 0)
		{
			utils::print_error(ERR_ALREADYREGISTRED, "Already registered");
			return (ERR_ALREADYREGISTRED);
		}
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
		{
			utils::print_error(ERR_ALREADYREGISTRED, "Already registered");
			return (ERR_ALREADYREGISTRED);
		}
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

vector<Client*>::iterator Command::
find_fd(vector<Client*>& vect, int fd)
{
	vector<Client*>::iterator v_begin	= vect.begin();

	while (v_begin != vect.end())
	{
		if ((*v_begin)->getSocketFd() == fd)
			return (v_begin);
		v_begin++;
	}
	return (vect.end());
}

/*
** ==================================================
** getCommand - возвращает текущую команду из буфера
** ==================================================
*/

const string& Command::
getCommand() const
{
	return (this->command);
}

bool Command::check_args_number(int n) const
{
	if ((int)this->arguments.size() < n)
	{
		utils::print_error(ERR_ARG_NUMBER, "Not enought parameters");
		return false;
	}
	return true;
}

bool Command::
check_password(const Client& client) const
{
	if (client.getPassword() == false)
	{
		utils::print_error(123, "ENTER PASS <password>");
		return false;
	}
	return true;
}

bool Command::
check_nickname(const Client& client) const
{
	if (client.getNickname().empty())
	{
		utils::print_error(ERR_NICKNAME, "ENTER NICK <nickname>");
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

int Command::
cmd_server(IRC& irc, int fd)
{
	vector<Client*>& vec_client 	= irc.get_clients();
	vector<User*>& vec_users		= irc.get_users();
	vector<Server*>& vec_server 	= irc.get_servers();
	vector<Client*>::iterator temp 	= this->find_fd(vec_client, fd);

	if (!this->check_args_number(3))
		return (0);

	if (IRC::find_fd(vec_server, fd) >= 0)
		return (ERR_ALREADYREGISTRED);

	if (temp == vec_client.end())
	{
		utils::print_error(ERR_FINDFD, "FD don't find in vector!");
		return 0;
	}

	if (!this->check_password(**temp))
		return 0;

	Server* new_server = new Server((*temp)->getSocketFd(), this->arguments[0], atoi(this->arguments[1].c_str()), this->arguments[2]);
	vec_server.push_back(new_server);
	irc.delete_client(fd);

	for (size_t i = 0; i < vec_users.size(); i++)
	{
		string temp_str = "NICK " + vec_users[i]->getNickname() + " " + utils::int_to_str(vec_users[i]->getHopcount()) + "\r\nUSER " + vec_users[i]->getUsername() + " " + vec_users[i]->getHostname() + " " + vec_users[i]->getServername() + " " + vec_users[i]->getRealname() + "\r\n";
		// irc.push_cmd_queue(new_server->getFdSocket(), temp_str);
	}
	

	return 0;
}
