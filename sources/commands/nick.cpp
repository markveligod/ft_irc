#include "Class.Command.hpp"
#include "Class.IRC.hpp"

/*
** ====================================================================
** Команда: NICK 
** ====================================================================
** Параметры: <nickname> [<hopcount>]
** ====================================================================
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
nick_available(std::vector<T> vect, const std::string& nick)
{
	for (size_t i = 0; i < vect.size(); i++)
	{
		if (vect[i]->getName() == nick)
		{
			utils::print_error(ERR_NICKNAME, "Nickname is already in use");
			return false;
		}
	}
	return true;
}

int Command::
nick_check_errors(int fd, int serv_client, IRC& irc)
{
	std::vector<Client *> &clients = irc.get_clients();
	// Если слишком много аргументов

	if (arguments.size() > 2)
	{
		utils::print_error(ERR_NEEDMOREPARAMS, "Too much arguments");
		irc.push_cmd_queue(fd, irc.response_2(ERR_NEEDMOREPARAMS, fd, "NICK", " :Syntax error"));
		return (ERR_NEEDMOREPARAMS);
	}

	// Если слишком мало аргументов
	if (arguments.size() == 0)
	{
		utils::print_error(ERR_NONICKNAMEGIVEN, "No nickname");
		irc.push_cmd_queue(fd, irc.response(ERR_NONICKNAMEGIVEN, fd, "*", ERR_NONICKNAMEGIVEN_MESS));
		return (ERR_NONICKNAMEGIVEN);
	}

	// Если 2 аргумента от клиента
	if (arguments.size() == 2 && serv_client < 0)
	{
		utils::print_error(ERR_NEEDMOREPARAMS, "Too much arguments");
		irc.push_cmd_queue(fd, irc.response_2(ERR_NEEDMOREPARAMS, fd, "NICK", " :Syntax error"));
		return (ERR_NEEDMOREPARAMS);
	}

	// Если 1 аргумент от сервера (и нет префикса)
	if (serv_client >= 0 && arguments.size() == 1 && prefix.empty())
	{
		utils::print_error(ERR_NEEDMOREPARAMS, "Too few arguments");
		irc.push_cmd_queue(fd, irc.response_2(ERR_NEEDMOREPARAMS, fd, "NICK", ERR_NEEDMOREPARAMS_MESS));
		return (ERR_NEEDMOREPARAMS);
	}

	// Если ник невалидный
	if (!this->nick_valid())
	{
		irc.push_cmd_queue(fd, irc.response(ERR_ERRONEUSNICKNAME, fd, arguments[0], ERR_ERRONEUSNICKNAME_MESS));
		return (ERR_ERRONEUSNICKNAME);
	}

	// Если этот ник уже использован и сообщение от сервера
	if (IRC::find_name(clients, this->arguments[0]) >= 0 && serv_client >= 0)
	{
		utils::print_error(ERR_NICKCOLLISION, "Nick collision (this nickname already in use)");
		irc.push_cmd_queue(fd, irc.response(ERR_NICKCOLLISION, fd, arguments[0], ERR_NICKCOLLISION_MESS));
		return (ERR_NICKCOLLISION);
	}

	// Если этот ник уже использован и сообщение от клиента
	if (IRC::find_name(clients, this->arguments[0]) >= 0 && serv_client < 0)
	{
		utils::print_error(ERR_NICKNAMEINUSE, "Nickname is already in use");
		irc.push_cmd_queue(fd, irc.response(ERR_NICKNAMEINUSE, fd, arguments[0], ERR_NICKNAMEINUSE_MESS));
		return (ERR_NICKNAMEINUSE);
	}

	// Если префикс не соответствует ни одному нику
	if (!prefix.empty() && (IRC::find_name(clients, this->prefix) < 0))
	{
		utils::print_error(0, "Invalid prefix \"" + prefix + "\"\r\n");
		irc.push_cmd_queue(fd, "ERROR :Invalid prefix \"" + prefix + "\"\r\n");
		return (1);
	}

	return (0);
}

int Command::
cmd_nick(IRC& irc, int fd)
{
	std::vector<Client*>&	clients 	= irc.get_clients();
	std::vector<User*>&		users 		= irc.get_users();
	std::vector<Server*>&	servers		= irc.get_servers();
	int						client_el	= IRC::find_fd(clients, fd);
	int						server_el	= IRC::find_fd(servers, fd);
	int						curr_client = -1;
	int						curr_user	= -1;
	int						error;
	std::stringstream		out_mess;

	if ((error = nick_check_errors(fd, server_el, irc)) != 0)
		return (error);

	// Если это от сервера
	if (server_el >= 0)
	{
		if (arguments.size() == 1)			// Изменение никнейма существующего клиента
		{
			curr_client = IRC::find_name(clients, prefix);
			clients[curr_client]->setNickname(arguments[0]);

			if ((curr_user = IRC::find_name(users, prefix)) >= 0)
				users[curr_user]->setNickname(arguments[0]);

			utils::print_line("Nickname for client changed from " + clients[curr_client]->getName() + " to " + this->arguments[0]);
			out_mess << "NICK " << arguments[0] << "\r\n";
		}
		else if (arguments.size() == 2)		// Создание нового клиента
		{
			if (!prefix.empty())
			{
				prefix = "";
				utils::print_error(0, "DEBUG Ignoring prefix (message from server with both prefix and hopcount).");
			}

			Client *new_client = new Client(fd, this->arguments[0], "" ,std::atoi(this->arguments[1].c_str()));
			clients.push_back(new_client);
			servers[server_el]->addClient(new_client);
			utils::print_line("New client created");
			curr_client = clients.size() - 1;
			out_mess << "NICK " << arguments[0] << " " << (clients[curr_client]->getHopcount() + 1) << "\r\n";
		}
		irc.forward_to_servers_2(fd, prefix, out_mess.str());
	}

	// Если это от клиента
	else if (client_el >= 0)
	{
		curr_client = client_el;
		if (!this->prefix.empty())
			curr_client = IRC::find_name(clients, this->prefix);

		// этот клиент еще не зарегестрирован
		if (clients[curr_client]->getName().empty())
			utils::print_line("Nickname for client set -> " + this->arguments[0]);
		// уже зарегестрирован
		else
		{
			if (prefix.empty())
				prefix = clients[curr_client]->getName();
			if ((curr_user = IRC::find_name(users, prefix)) >= 0)
			{
				users[curr_user]->setNickname(arguments[0]);
				irc.push_cmd_queue(fd, irc.full_name(users[curr_user]) + " NICK :" + this->arguments[0] + "\r\n");
			}
			out_mess << "NICK " << arguments[0] << "\r\n";
			irc.forward_to_servers_2(fd, prefix, out_mess.str());

			utils::print_line("Nickname changed " + clients[curr_client]->getName() + " -> " + this->arguments[0]);
		}
		
		clients[curr_client]->setNickname(this->arguments[0]);
	}

	return 0;
}

//User *user = irc.get_user(clients[curr_client]);
//irc.push_cmd_queue(fd, irc.full_name(user) + " NICK :" + this->arguments[0] + "\r\n");
