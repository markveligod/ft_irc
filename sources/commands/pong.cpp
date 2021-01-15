#include "main.hpp"
#include "Class.Command.hpp"

/*
** ====================================================================
** Команда: PONG 
** Параметры: <server> [<server2>]
** ====================================================================
** PONG является ответной реакцией на PING. Если дан параметр <server2>, 
** это сообщение должно быть перенаправленна данному серверу. Параметр 
** <server> является именем сервера, который отвечает на PING-сообщение
** и генерирует это сообщение. 
** ====================================================================
*/

int Command::
cmd_pong(IRC& irc, int fd)
{
	string sender_name = (is_server(irc, fd))
					? irc.get_server_name(fd)
					: irc.get_nickname(fd);
	
	if (arguments.empty())
	{
		irc.push_cmd_queue(fd, irc.response_to_client(ERR_NOORIGIN, fd, sender_name, ERR_NOORIGIN_MESS));
		return 1;
	}

	if (arguments.size() == 1)
		return 0;

	// Если 2 аргумента, переправляем PING дальше
	
	int i = IRC::find_name(irc.get_servers(), arguments[0]);		// ищем адресата среди серверов
	if (i < 0)
		i = IRC::find_name(irc.get_users(), arguments[0]);			// ищем среди юзеров, если не нашли среди серверов
	else
	{
		Server* server = irc.get_servers()[i];
		irc.push_cmd_queue(server->getSocketFd(),
							":" + arguments[0] +
							" PONG " +
							server->getName() +
							" :" + arguments[1]);
		return 0;
	}
	if (i < 0)
	{
		irc.push_cmd_queue(fd, irc.response_to_client(ERR_NOSUCHSERVER, fd, sender_name, ERR_NOSUCHSERVER_MESS));
		return 1;
	}
	else
	{
		User* user = irc.get_users()[i];
		irc.push_cmd_queue(user->getSocketFd(),
							":" + arguments[0] +
							" PONG " +
							user->getUsername() +
							" :" + arguments[1]);
	}
	return 0;
}
