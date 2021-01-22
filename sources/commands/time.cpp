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

void Command::
cmd_time(IRC& irc, int fd)
{
	if (_arguments.size() > 1)												// слишком много аргуменотов
	{
		irc.push_cmd_queue(fd, irc.response(ERR_NEEDMOREPARAMS, fd, _command, ERR_NEEDMOREPARAMS_MESS));
		return;
	}

	User* user = _prefix.size() ? irc.get_user(_prefix) : irc.get_user(fd);
	if (!user)																// сообщение от нусществующего пользователя
	{
		irc.push_cmd_queue(fd, irc.response(ERR_NOSUCHNICK, fd, _command, ERR_NOSUCHNICK_MESS));
		return;
	}


	string target = string();
	
	if (_arguments.empty())
		target = irc.get_server_name();
	else if (IRC::find_name(irc.get_servers(), _arguments[0]) >= 0)
		target = _arguments[0];
	else if (IRC::find_name(irc.get_users(), _arguments[0]) >= 0)
		target = irc.get_user(_arguments[0])->getServername();
	else
	{
		irc.push_cmd_queue(fd, irc.response(ERR_NOSUCHSERVER, fd, _command, ERR_NOSUCHSERVER_MESS));
		return;
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
}
