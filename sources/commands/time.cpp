#include "Class.Command.hpp"
#include "Class.IRC.hpp"
#include "utils.hpp"

/*
** ====================================================================
** Команда: TIME
** ====================================================================
** Параметры:
** ====================================================================
** 
** =====================================================================
*/

int Command::cmd_time(IRC& irc, int fd)
{
	if (arguments.size() > 1)												// слишком много аргуменотов
	{
		irc.push_cmd_queue(fd, irc.response(ERR_NEEDMOREPARAMS, fd, command, ERR_NEEDMOREPARAMS_MESS));
		return 1;
	}

	User* user = prefix.size() ? irc.get_user(prefix) : irc.get_user(fd);
	if (!user)																// сообщение от нусществующего пользователя
	{
		irc.push_cmd_queue(fd, irc.response(ERR_NOSUCHNICK, fd, command, ERR_NOSUCHNICK_MESS));
		return 1;
	}


	string target = string();
	
	if (arguments.empty())
		target = irc.get_server_name();
	else if (IRC::find_name(irc.get_servers(), arguments[0]) >= 0)
		target = arguments[0];
	else if (IRC::find_name(irc.get_users(), arguments[0]) >= 0)
		target = irc.get_user(arguments[0])->getServername();
	else
	{
		irc.push_cmd_queue(fd, irc.response(ERR_NOSUCHSERVER, fd, command, ERR_NOSUCHSERVER_MESS));
		return 1;
	}
	
	time_t t = time(0);
	string time_mess = ctime(&t);
	time_mess.erase(--time_mess.end());

	string message = ":" +
						target + " " +
						utils::int_to_str(RPL_TIME) + " " +
						user->getName() + " " +
						target + " :" +
						time_mess + "\r\n";

	irc.push_cmd_queue(fd, message);

	return 0;
}
