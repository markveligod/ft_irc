#include "Class.Command.hpp"
#include "Class.IRC.hpp"
#include "utils.hpp"

/*
** ====================================================================
** Команда: SQUIT
** ====================================================================
** Параметры: <server> <comment> 
** ====================================================================
** Сообщение SQUIT требуется для указания мертвых или вышедших серверов. 
** Если сервер желает оборвать соединение с другим сервером, он должен 
** послать сообщение SQUIT другому серверу, используя имя другого сервера 
** в качестве параметра server. 
** Эта команда так же доступна IRC-операторам для помощи в сохранеии сети 
** соединений IRC-серверов в порядке. IRC-операторы могут так же послать 
** SQUIT для удаленных серверных соединений. В этом случае, SQUID будет 
** парситься каждым сервером, находящимся между IRC-оператором и удаленным 
** сервером. 
** Параметр <comment> обеспечивается всеми операторами, которые запускают 
** SQUIT для удаленных серверов (которые не присоединены к серверу, 
** который хотят выключить), так что все операторы знают причины этого 
** действия. 
** Один из серверов, которые находятся на другой стороне соединения, будет 
** закрыт по требованию, высланным сообщением SQUID (ко всем другим 
** соединениям) дл остальных серверво, которые рассматриваются как линки. 
** Подобным образом, SQUIT может быть послана другим серверам, находящимся 
** в сети ради клиентов. В дополнение к этому, все члены канала, который 
** разбило сплитом, может послать SQUIT-сообщение. 
** Если соединение сервера закрыто преждевременно (т.е сервер на другом 
** конце соединения разорвал линк), сервер, который засек этот разрыв 
** соединения, информирует всю сеть о том, что соединение закрыто и 
** показывает поле <comment>, обьясняя причину рассоединения. 
** =====================================================================
*/

void Command::
cmd_squit(IRC& irc, int fd)
{
	string sender = (!_prefix.empty()) ? _prefix : irc.get_user(fd)->getName();

	if (!irc.is_server(fd) && !irc.get_user(fd)->getMode('o'))	// сообщение от пользовател, не являющегося оператором сервера
	{
		irc.push_cmd_queue(fd, irc.response(ERR_NOPRIVILEGES, sender, _command, ERR_NOPRIVILEGES_MESS));
		return;
	}

	if (!check_args_number(2))
	{
		irc.push_cmd_queue(fd, irc.response(ERR_NEEDMOREPARAMS, sender, _command, ERR_NEEDMOREPARAMS_MESS));
		return;
	}

	string quit_server_name = _arguments[0];
	Server* quit_server = irc.get_server(quit_server_name);

	if (!quit_server)
	{
		irc.push_cmd_queue(fd, irc.response(ERR_NOSUCHSERVER, sender, _command, ERR_NOSUCHSERVER_MESS));
		return;
	}

	std::vector<User*>& users = irc.get_users();
	for (size_t i = 0; i < users.size(); i++)
	{
		if ((quit_server->getHopcount() == 1								// если удаяемы сервер подключен к нам напрямую
				&& users[i]->getSocketFd() == quit_server->getSocketFd())	// то удаляем всех пользователей в цепочке серверов (по fd)
			|| users[i]->getServername() == quit_server_name)
			irc.delete_user(users[i]);										// удаляем пользователя из всех каналов, из _users и из _clients 
	}

	string wallop = ":" + irc.get_server_name() +
								" WALLOPS :Received SQUIT " +
								quit_server_name +
								" from " +
								sender + ": " +
								_arguments[1];

	irc.forward_to_servers(fd, wallop);				// рассылаем валлоп серверам

													// и операторам текущего сервера
	for (vector<User*>::iterator it = users.begin(); it != users.end(); it++)
	{
		if ((*it)->getHopcount() == 0 && ((*it)->getMode('o') || (*it)->getMode('w')))
			irc.push_cmd_queue(fd, wallop + "\r\n");
	}

	string quit_msg2 = ":" + quit_server_name +
								" SQUIT " +
								quit_server_name + " :" +
								_arguments[1] +
								" (SQUIT from " +
								sender + ")";

	irc.forward_to_servers(fd, quit_msg2);

	irc.close_connection(quit_server);
	
}
