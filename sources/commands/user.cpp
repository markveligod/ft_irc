#include "main.hpp"
#include "Class.Command.hpp"

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
	std::cout << "DEBUG   users size = " << users.size() << std::endl;
	std::cout << "UserName: " << curr_user->getUsername() << std::endl;
	std::cout << "HostName: " << curr_user->getHostname() << std::endl;
	std::cout << "ServerName: " << curr_user->getServername() << std::endl;
	std::cout << "RealName: " << curr_user->getRealname() << std::endl;
	std::cout << "NickName: " << curr_user->getNickname() << std::endl;
	std::cout << "HopCount: " << curr_user->getHopcount() << std::endl;

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

	if (arguments[0][0] == '~')
		arguments[0] = arguments[0].substr(1, arguments[0].size());

	if (!(this->prefix.empty()) &&								// если префикс есть
		(server_fd = IRC::find_fd(servers, fd)) >= 0 &&			// и сообщение пришло с сервера
		(i = IRC::find_name(clients, this->prefix)) >= 0)		// и есть клиент с таким ником
	{
		if (!(check_nickname(*clients[i])))						// и ввел ли клиент ник
			return 0;
		if (IRC::find_name(users, this->prefix) >= 0)			// если уже есть юзер с таким никнеймом
		{
			utils::print_error(ERR_ALREADYREGISTRED, "Already registered");
			return (ERR_ALREADYREGISTRED);
		}
		else
			this->user_create(clients[i], users, servers[server_fd]);
	}

	else if (this->prefix.empty() &&									// если префикс пуст
	   		(i = IRC::find_fd(clients, fd)) >= 0)						// и сообщение от клиента
	{
		if (!(check_password(*clients[i])) ||						// проверяем, ввел ли клиент пароль
			!(check_nickname(*clients[i])))							// и ввел ли клиент ник
			return 0;
		if (IRC::find_name(users, clients[i]->getName()) >= 0)
		{
			utils::print_error(ERR_ALREADYREGISTRED, "Already registered");
			return (ERR_ALREADYREGISTRED);
		}
		else
			this->user_create(clients[i], users, NULL);
	}
	else
		return 0;

	User *curr_user = users[users.size() - 1];
	std::stringstream ss;
	ss << "NICK " << curr_user->getNickname() << " " << (curr_user->getHopcount() + 1);
	irc.forward_message_to_servers_2(fd, "", ss.str());
	irc.forward_message_to_servers(fd, message, !prefix.empty());

	return 0;
}