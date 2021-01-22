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
cmd_user(IRC& irc, int fd)
{
	vector<Client*>& clients	= irc.get_clients();
	vector<User*>& users 		= irc.get_users();
	vector<Server*>& servers 	= irc.get_servers();
	int server_el				= IRC::find_fd(servers, fd);
	int server_el_name			= irc.find_name(servers, _arguments[2]);
	int client_el				= IRC::find_fd(clients, fd);
	int error;

	if ((error = user_check_errors(irc, fd)) != 1)
		return;

	if (_arguments[0][0] == '~')
		_arguments[0] = _arguments[0].substr(1, _arguments[0].size());

	// Если сообщение от сервера
	if (server_el >= 0)
	{
		if (server_el_name < 0)
			_arguments[2] = irc.get_server_name();
		client_el = IRC::find_name(clients, _prefix);
		if (server_el_name < 0)
			user_create(clients[client_el], users, servers[server_el]);
		else
			user_create(clients[client_el], users, servers[server_el_name]);
	}
	else
	{
		_arguments[1] = clients[client_el]->getHostname();
		_arguments[2] = irc.get_server_name();
		user_create(clients[client_el], users, NULL);
	}
	
	User *curr_user = users[users.size() - 1];
	std::stringstream out_message;
	out_message << "NICK " << curr_user->getNickname() << " " << (curr_user->getHopcount() + 1) << "\r\n";
	out_message << ":" << curr_user->getNickname() << " USER "
				<< curr_user->getUsername() << " "
				<< curr_user->getHostname() << " "
				<< curr_user->getServername() << " :"
				<< curr_user->getRealname();
	irc.forward_to_servers(fd, out_message.str());

	//hello
	if (server_el < 0)
	{
		irc.push_cmd_queue(fd, irc.response(001, curr_user->getNickname(), "001", "Welcome to the Internet Relay Network " + irc.fullname(curr_user)));

		irc.push_cmd_queue(fd, irc.response(002, curr_user->getNickname(), "002", "Your host is " + irc.get_server_name() + ", running version " + VERSION));

		time_t t = time(0);
		string time_mess = ctime(&t);
		time_mess.erase(--time_mess.end());
		irc.push_cmd_queue(fd, irc.response(003, curr_user->getNickname(), "003", "This server was created " + time_mess));

		irc.push_cmd_queue(fd, irc.response(004, curr_user->getNickname(), "004", irc.get_server_name() + " " + VERSION + "aiwroO" + "OovaimnqpsrtklbeI"));

		irc.push_cmd_queue(fd, irc.response(005, curr_user->getNickname(), "005", "Try server " + irc.get_server_name()));

		irc.push_cmd_queue(fd, irc.response(251, curr_user->getNickname(), "251", ":There are " + utils::int_to_str(users.size()) + " users and 0 services on " + utils::int_to_str(servers.size()) + " servers"));

		irc.push_cmd_queue(fd, irc.response(254, curr_user->getNickname(), "254", utils::int_to_str(irc.get_channels().size()) + " :channels formed"));

		irc.push_cmd_queue(fd, irc.response(255, curr_user->getNickname(), "255", ":I have " + utils::int_to_str(users.size()) + " clients and " + utils::int_to_str(servers.size()) + " servers"));

		irc.push_cmd_queue(fd, irc.response(375, curr_user->getNickname(), "375", ":- " + irc.get_server_name() + " Message of the day - "));

		vector<string> temp_motd = irc.motd_generate();
		for (size_t i = 0; i < temp_motd.size(); i++)
		{
			irc.push_cmd_queue(fd, irc.response(372, curr_user->getNickname(), "372", temp_motd[i]));
		}

		irc.push_cmd_queue(fd, irc.response(376, curr_user->getNickname(), "376", ":End of MOTD command"));
	}
}

void Command::
user_create(Client* curr_client, vector<User*>& users, Server* curr_server)
{
	User* curr_user = new User(curr_client);
	curr_user->user_from_client(_arguments[0], _arguments[1],
								_arguments[2], _arguments[3]);
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

	// Если пришло от сервера
	if (server_el >= 0)
	{
		// Если префикс пуст
		if (_prefix.empty())
		{
			utils::print_error(0, "Empty prefix");
			irc.push_cmd_queue(fd, "ERROR :Empty prefix\r\n");
			return 0;
		}
		if (_arguments.size() != 4)
		{
			utils::print_error(ERR_NEEDMOREPARAMS, "Invalid number of parameters");
			irc.push_cmd_queue(fd, irc.response(ERR_NEEDMOREPARAMS, _prefix, "USER", ":Syntax error"));
			return (ERR_NEEDMOREPARAMS);
		}
		// Если нет клиента с ником - префиксом
		if ((i = irc.find_name(clients, _prefix)) < 0)
		{
			utils::print_error(0, "Unknown nickname in prefix");
			irc.push_cmd_queue(fd, "ERROR :Unknown nickname in prefix\r\n");
			return 0;
		}
		// Если пришло не с того сервера, который владеет клиентом
		if (clients[i]->getSocketFd() != fd)
		{
			utils::print_error(0, "This server doesn't own user with such nickname");
			irc.push_cmd_queue(fd, "ERROR :This server doesn't own user with such nickname\r\n");
			return 0;
		}
		// Если этот пользователь уже зарегестрирован
		if (irc.find_name(users, _prefix) >= 0)
		{
			utils::print_error(ERR_ALREADYREGISTRED, "Already registered");
			irc.push_cmd_queue(fd, irc.response(ERR_ALREADYREGISTRED, _prefix, "", ERR_ALREADYREGISTRED_MESS));
			return (ERR_ALREADYREGISTRED);
		}
	}
	else if (client_el >= 0)
	{
		if (!check_password(*clients[client_el]) ||
			!check_nickname(*clients[client_el]))
		{
			irc.push_cmd_queue(fd, "ERROR :You are not registered\r\n");
			return 0;
		}
		if (irc.find_name(users, clients[client_el]->getName()) >= 0)
		{
			utils::print_error(ERR_ALREADYREGISTRED, "Already registered");
			irc.push_cmd_queue(fd, irc.response(ERR_ALREADYREGISTRED, clients[client_el]->getName(), "", ERR_ALREADYREGISTRED_MESS));
			return (ERR_ALREADYREGISTRED);
		}
		if (_arguments.size() != 4)
		{
			utils::print_error(ERR_NEEDMOREPARAMS, "Invalid number of parameters");
			irc.push_cmd_queue(fd, irc.response(ERR_NEEDMOREPARAMS, clients[client_el]->getName(), "USER", ERR_NEEDMOREPARAMS_MESS));
			return (ERR_NEEDMOREPARAMS);
		}
	}
	else
	{
		utils::print_error(0, "Message from unregistered connection!");
		irc.push_cmd_queue(fd, "ERROR :Message from unregistered connection!\r\n");
		return 0;
	}
	return (1);
}
