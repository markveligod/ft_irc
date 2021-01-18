#include "Class.Command.hpp"
#include "Class.IRC.hpp"
#include "utils.hpp"

/*
** ====================================================================
** Команда: AWAY
** ====================================================================
** Параметры: [message]
** ====================================================================
** С сообщением AWAY, клиенты могут устанавливать автоматическую строку 
** ответа на любые PRIVMSG-команды, направленные им (не на канал). 
** Автоматический ответ посылается сервером к клиенту, пославшего команду 
** PRIVMSG. Только отвечающий сервер может быть только один, к которому 
** подсоединен клиент. 

** AWAY используется вместе с одним параметром (установка сообщения AWAY) 
** или без параметров (снятие сообщения AWAY). 
** =====================================================================
*/

int Command::cmd_away(IRC& irc, int fd)
{
	User* user = (prefix.size()) ? irc.get_user(prefix) : irc.get_user(fd);
	if (!user) return 1;

	string forward = ":" + user->getName() + " MODE " + user->getName();
	
	if (arguments.empty())
	{
		user->setMode('a', false);

		string replay = irc.response(RPL_UNAWAY, fd, "", RPL_UNAWAY_MESS);
		
		if (!irc.is_server(fd))
			irc.push_cmd_queue(fd, replay);
		
		irc.forward_to_servers(fd, forward + " :-a", true);
	}

	if (arguments.size() == 1)
	{
		user->setMode('a', true);
		user->setAway(" " + arguments[0]);

		string replay = irc.response(RPL_NOWAWAY, fd, "", RPL_NOWAWAY_MESS);

		if (!irc.is_server(fd))
			irc.push_cmd_queue(fd, replay);

		irc.forward_to_servers(fd, forward + " :+a", true);
	}
	return 0;
}
