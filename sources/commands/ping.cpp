#include "main.hpp"
#include "Class.Command.hpp"

/*
** ====================================================================
** Команда: PONG 
** ====================================================================
** Параметры: <server> [<server2>]
** ====================================================================
** PING используется для тестирования наличия активности клиента на другом 
** конце соединения. Это сообщение посылается с регулярными интервалами, 
** если не замечено другой активности, исходящей от соединения. Если 
** соединение не отвечает на PING - соединение закрыто. 
** Любой клиент, который получил PING-сообщение, должен ответить на 
** <server1> (сервер, который посылает сообщение PING) так быстро, как это 
** только возможно, с PONG-сообщением, указывая на то, что он еще здесь и 
** живой. Серверы могут не отвечать на команды PING, но полагаясь на 
** PING'и с другого конца соединения, устанавливают живо ли соединение. 
** Если указан параметр <server2>, PING'сообщение перенаправляется туда. 
** ====================================================================
*/

void Command::
cmd_ping(IRC& irc, int fd)
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
	{
		string name = irc.get_server_name();
		irc.push_cmd_queue(fd, ":" + name + " PONG " + sender_name + " :" + _arguments[0] + "\r\n");

		return;
	}
	
	// Если 2 аргумента, переправляем PING дальше
	
	int i = IRC::find_name(irc.get_servers(), _arguments[0]);		// ищем адресата среди серверов
	if (i < 0)
		i = IRC::find_name(irc.get_users(), _arguments[0]);			// ищем среди юзеров, если не нашли среди серверов
	else
	{
		Server* server = irc.get_servers()[i];
		irc.push_cmd_queue(server->getSocketFd(),
							":" + _arguments[0] +
							" PING " +
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
							" PING " +
							user->getUsername() +
							" :" + _arguments[1] + "\r\n");
	}
}
