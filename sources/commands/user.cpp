#include "main.hpp"
#include "Class.Command.hpp"

/*
** =============================================================
** Команда: USER 
** =============================================================
** Параметры: <username> <hostname> <servername> <realname> 
** =============================================================
** cmd_user - при удачном вызове дозополняет класс Client
** =============================================================
*/

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
user_check_errors(IRC& irc, int fd)
{
	vector<Client *> &clients	= irc.get_clients();
	vector<Server*>& servers 	= irc.get_servers();
	vector<User *> &users		= irc.get_users();
	int server_el				= IRC::find_fd(servers, fd);
	int client_el				= IRC::find_fd(clients, fd);
	int i;

	if (!(this->check_args_number(4)))
		return (ERR_NEEDMOREPARAMS);
	// Если пришло от сервера
	if (server_el >= 0)
	{
		// Если префикс пуст
		if (this->prefix.empty())
		{
			utils::print_error(0, "Empty prefix");
			irc.push_cmd_queue(fd, "ERROR :Empty prefix\r\n");
			return (0);
		}
		// Если нет клиента с ником - префиксом
		if ((i = irc.find_name(clients, this->prefix)) < 0)
		{
			utils::print_error(0, "Unknown nickname in prefix");
			irc.push_cmd_queue(fd, "ERROR :Unknown nickname in prefix\r\n");
			return (0);
		}
		// Если пришло не с того сервера, который владеет клиентом
		if (clients[i]->getSocketFd() != fd)
		{
			utils::print_error(0, "This server doesn't own user with such nickname");
			irc.push_cmd_queue(fd, "ERROR :This server doesn't own user with such nickname\r\n");
			return (0);
		}
		// Если этот пользователь уже зарегестрирован
		if (irc.find_name(users, this->prefix) >= 0)
		{
			utils::print_error(ERR_ALREADYREGISTRED, "Already registered");
			irc.push_cmd_queue(fd, irc.response(ERR_ALREADYREGISTRED, fd, arguments[0], ERR_ALREADYREGISTRED_MESS));
			return (ERR_ALREADYREGISTRED);
		}
	}
	else if (client_el >= 0)
	{
		if (!check_password(*clients[client_el]) ||
			!check_nickname(*clients[client_el]))
		{
			irc.push_cmd_queue(fd, "ERROR :You are not registered\r\n");
			return (0);
		}
		if (irc.find_name(users, clients[client_el]->getName()) >= 0)
		{
			utils::print_error(ERR_ALREADYREGISTRED, "Already registered");
			irc.push_cmd_queue(fd, irc.response(ERR_ALREADYREGISTRED, fd, arguments[0], ERR_ALREADYREGISTRED_MESS));
			return (ERR_ALREADYREGISTRED);
		}
	}
	else
	{
		utils::print_error(0, "Message from unregistered connection!");
		irc.push_cmd_queue(fd, "ERROR :Message from unregistered connection!\r\n");
		return (0);
	}
	return (1);
}

int Command::
cmd_user(IRC& irc, int fd)
{
	vector<Client*>& clients	= irc.get_clients();
	vector<User*>& users 		= irc.get_users();
	vector<Server*>& servers 	= irc.get_servers();
	int server_el				= IRC::find_fd(servers, fd);
	int client_el				= IRC::find_fd(clients, fd);
	int error;

	if ((error = user_check_errors(irc, fd)) != 1)
		return (error);

	if (arguments[0][0] == '~')
		arguments[0] = arguments[0].substr(1, arguments[0].size());

	// Если сообщение от сервера
	if (server_el >= 0)
	{
		if (irc.find_name(servers, arguments[2]) <= 0 && irc.find_name(servers, arguments[1]) <= 0)
			arguments[2] = irc.get_server_name();
		client_el = IRC::find_name(clients, this->prefix);
		this->user_create(clients[client_el], users, servers[server_el]);
	}
	else
	{
		arguments[1] = clients[client_el]->getHostname();
		arguments[2] = irc.get_server_name();
		this->user_create(clients[client_el], users, NULL);
	}

	User *curr_user = users[users.size() - 1];
	std::stringstream out_message;
	out_message << "NICK " << curr_user->getNickname() << " " << (curr_user->getHopcount() + 1) << "\r\n";
	out_message << ":" << curr_user->getNickname() << " USER "
				<< curr_user->getUsername() << " "
				<< curr_user->getHostname() << " "
				<< curr_user->getServername() << " :"
				<< curr_user->getRealname();
	irc.forward_to_servers_2(fd, "", out_message.str());

	return 0;
}