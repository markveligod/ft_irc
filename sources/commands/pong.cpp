#include "main.hpp"
#include "Class.Command.hpp"

/*
** ====================================================================
** Команда: PONG 
** ====================================================================
** Параметры: <server> [<server2>]
** ====================================================================
** PONG является ответной реакцией на PING. Если дан параметр <server2>, 
** это сообщение должно быть перенаправленна данному серверу. Параметр 
** <server> является именем сервера, который отвечает на PING-сообщение
** и генерирует это сообщение. 
** ====================================================================
*/

void Command::
cmd_pong(IRC& irc, int fd)
{
	string sender_name = (is_server(irc, fd))
					? irc.get_server_name(fd)
					: irc.get_nickname(fd);
	
	if (_arguments.empty())
	{
		irc.push_cmd_queue(fd, irc.response(ERR_NOORIGIN, fd, sender_name, ERR_NOORIGIN_MESS));
		return;
	}

	if (_arguments.size() == 1)
		return;

	// Если 2 аргумента, переправляем PING дальше
	
	int i = IRC::find_name(irc.get_servers(), _arguments[0]);		// ищем адресата среди серверов
	if (i < 0)
		i = IRC::find_name(irc.get_users(), _arguments[0]);			// ищем среди юзеров, если не нашли среди серверов
	else
	{
		Server* server = irc.get_servers()[i];
		irc.push_cmd_queue(server->getSocketFd(),
							":" + _arguments[0] +
							" PONG " +
							server->getName() +
							" :" + _arguments[1] + "\r\n");
		return;
	}
	if (i < 0)
	{
		irc.push_cmd_queue(fd, irc.response(ERR_NOSUCHSERVER, fd, sender_name, ERR_NOSUCHSERVER_MESS));
		return;
	}
	else
	{
		User* user = irc.get_users()[i];
		irc.push_cmd_queue(user->getSocketFd(),
							":" + _arguments[0] +
							" PONG " +
							user->getUsername() +
							" :" + _arguments[1] + "\r\n");
	}
}
