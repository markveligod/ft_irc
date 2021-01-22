#include "Class.Command.hpp"
#include "Class.IRC.hpp"
#include "utils.hpp"

/*
** ====================================================================
** Команда: CONNECT
** ====================================================================
** Параметры: <target server> [<port>]
** ====================================================================
** Команда используется для попытки создания сервером нового соединения с 
** другим сервером. CONNECT-сообщением могут пользоваться только 
** IRC-операторы. Если удаленный сервер указан в строке параметров, он 
** присоединяется к <target server> на <port>.
** =====================================================================
*/

void Command::
cmd_connect(IRC& irc, int fd)
{
	string recepient = (!_prefix.empty()) ? _prefix : irc.get_user(fd)->getName();

	std::cout << _arguments.size() << std::endl;
	if (!(_arguments.size() == 1 || _arguments.size() == 2))
	{
		irc.push_cmd_queue(fd, irc.response(ERR_NEEDMOREPARAMS, recepient, _command, ERR_NEEDMOREPARAMS_MESS));
		return;
	}

	User* user = irc.get_user(fd);
	if (!user || !user->getMode('o'))
	{
		irc.push_cmd_queue(fd, irc.response(ERR_NOPRIVILEGES, recepient, _command, ERR_NOPRIVILEGES_MESS));
		return;
	}

	irc.add_network_ip(_arguments[0]);
	irc.add_network_port(_arguments.size() == 2 ? _arguments[1] : DEF_PORT);
	irc.add_network_pass(DEF_PASS);

	irc.create_socket_network();
}
