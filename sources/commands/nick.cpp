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
void Command::
cmd_nick(IRC& irc, int fd)
{
	std::vector<Client*>&	clients 	= irc.get_clients();
	std::vector<User*>&		users 		= irc.get_users();
	std::vector<Server*>&	servers		= irc.get_servers();
	int						client_el	= IRC::find_fd(clients, fd);
	int						server_el	= IRC::find_fd(servers, fd);
	int						curr_client = -1;
	int						curr_user	= -1;
	std::stringstream		out_mess;

	if ((nick_check_errors(fd, server_el, irc) != 0))
		return;

	// Если это от сервера
	if (server_el >= 0)
	{
		if (_arguments.size() == 1)			// Изменение никнейма существующего клиента
		{
			curr_client = IRC::find_name(clients, _prefix);
			clients[curr_client]->setNickname(_arguments[0]);

			if ((curr_user = IRC::find_name(users, _prefix)) >= 0)
			{
				irc.push_cmd_queue(fd, irc.fullname(users[curr_user]) + " NICK :" + _arguments[0] + "\r\n");
				irc.forward_to_all_channels(users[curr_user], irc.fullname(users[curr_user]) + " NICK :" + _arguments[0]);
				users[curr_user]->setNickname(_arguments[0]);
			}

			utils::print_line("Nickname for client changed from " + _prefix + " to " + _arguments[0]);
			out_mess << ":" << _prefix << " NICK " << _arguments[0];
			irc.forward_to_servers(fd, out_mess.str());
		}
		else if (_arguments.size() == 2)		// Создание нового клиента
		{
			if (!_prefix.empty())
			{
				_prefix = "";
				utils::print_error(0, "DEBUG Ignoring _prefix (message from server with both _prefix and hopcount).");
			}

			Client *new_client = new Client(fd, _arguments[0], "" ,std::atoi(_arguments[1].c_str()));
			clients.push_back(new_client);
			servers[server_el]->addClient(new_client);
			utils::print_line("New client created");
			curr_client = clients.size() - 1;
		}
	}

	// Если это от клиента
	else if (client_el >= 0)
	{
		curr_client = client_el;
		if (!_prefix.empty())
			curr_client = IRC::find_name(clients, _prefix);

		// этот клиент еще не зарегестрирован
		if (clients[curr_client]->getName().empty())
			utils::print_line("Nickname for client set -> " + _arguments[0]);
		// уже зарегестрирован
		else
		{
			if (_prefix.empty())
				_prefix = clients[curr_client]->getName();
			if ((curr_user = IRC::find_name(users, _prefix)) >= 0)
			{
				irc.push_cmd_queue(fd, irc.fullname(users[curr_user]) + " NICK :" + _arguments[0] + "\r\n");
				irc.forward_to_all_channels(users[curr_user], irc.fullname(users[curr_user]) + " NICK :" + _arguments[0]);
				users[curr_user]->setNickname(_arguments[0]);
			}
			out_mess << ":" << _prefix << " NICK :" << _arguments[0];
			irc.forward_to_servers(fd, out_mess.str());

			utils::print_line("Nickname changed " + clients[curr_client]->getName() + " -> " + _arguments[0]);
		}
		clients[curr_client]->setNickname(_arguments[0]);
	}
}

bool Command::
nick_valid() const
{
	int i = 1;
	if (_arguments[0].size() > 9 || _arguments[0].size() == 0)
	{
		utils::print_error(ERR_NICKNAME, "Nickname length must be between 1 and 9 symbols");
		return false;
	}
	if (!(std::isalpha(_arguments[0][0])))
	{
		utils::print_error(ERR_NICKNAME, "First symbol must be a letter");
		return false;
	}
	while (i != (int)_arguments[0].size())
	{
		if (std::isalnum(_arguments[0][i]) ||
			std::strchr("-[]\\`^{}", _arguments[0][i]) != NULL)
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
		if (is_equal(vect[i]->getName(), nick))
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
	std::vector<Client *> &clients	= irc.get_clients();
	int		client_el 				= irc.find_fd(clients, fd);
	string	nick_name				= (clients[client_el]->getName().empty() ? "*" : clients[client_el]->getName());

	// (Если слишком много аргументов) или (Если 2 аргумента от клиента) или (Если 1 аргумент от сервера (и нет префикса))
	if (_arguments.size() > 2 ||
		(_arguments.size() == 2 && serv_client < 0) ||
		(serv_client >= 0 && _arguments.size() == 1 && _prefix.empty()))
	{
		utils::print_error(ERR_NEEDMOREPARAMS, "Too much _arguments");
		irc.push_cmd_queue(fd, irc.response(ERR_NEEDMOREPARAMS, nick_name, "NICK", ERR_NEEDMOREPARAMS_MESS));
		return (ERR_NEEDMOREPARAMS);
	}

	// Если слишком мало аргументов
	if (_arguments.size() == 0)
	{
		utils::print_error(ERR_NONICKNAMEGIVEN, "No nickname");
		irc.push_cmd_queue(fd, irc.response(ERR_NONICKNAMEGIVEN, nick_name, "NICK", ERR_NONICKNAMEGIVEN_MESS));
		return (ERR_NONICKNAMEGIVEN);
	}

	// Если ник невалидный
	if (!nick_valid())
	{
		irc.push_cmd_queue(fd, irc.response(ERR_ERRONEUSNICKNAME, nick_name, _arguments[0], ERR_ERRONEUSNICKNAME_MESS));
		return (ERR_ERRONEUSNICKNAME);
	}

	// Если этот ник уже использован и сообщение от сервера
	if (IRC::find_name(clients, _arguments[0]) >= 0 && serv_client >= 0)
	{
		utils::print_error(ERR_NICKCOLLISION, "Nick collision (this nickname already in use)");
		irc.push_cmd_queue(fd, irc.response(ERR_NICKCOLLISION, nick_name, _arguments[0], ERR_NICKCOLLISION_MESS));
		irc.push_cmd_queue(fd, ":" + irc.get_server_name() + " KILL " + _arguments[0] + " " + _arguments[1] + "\r\n");

		return (ERR_NICKCOLLISION);
	}

	// Если этот ник уже использован и сообщение от клиента
	if (IRC::find_name(clients, _arguments[0]) >= 0 && serv_client < 0)
	{
		utils::print_error(ERR_NICKNAMEINUSE, "Nickname is already in use");
		irc.push_cmd_queue(fd, irc.response(ERR_NICKNAMEINUSE, nick_name, _arguments[0], ERR_NICKNAMEINUSE_MESS));
		return (ERR_NICKNAMEINUSE);
	}

	// Если префикс не соответствует ни одному нику
	if (!_prefix.empty() && (IRC::find_name(clients, _prefix) < 0))
	{
		utils::print_error(0, "Invalid _prefix \"" + _prefix + "\"\r\n");
		irc.push_cmd_queue(fd, "ERROR :Invalid _prefix \"" + _prefix + "\"\r\n");
		return (1);
	}

	return 0;
}


//User *user = irc.get_user(clients[curr_client]);
//irc.push_cmd_queue(fd, irc.fullname(user) + " NICK :" + _arguments[0] + "\r\n");
