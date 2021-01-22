#include "Class.Command.hpp"
#include "Class.IRC.hpp"
#include "utils.hpp"

/*
** ====================================================================
** Команда: KILL
** ====================================================================
** Параметры: <nickname> <comment>
** ====================================================================
** Отправка сообщения всем IRC-операторам, которые находятся в данный 
** момент в сети. После выполнения WALLOPS как команды пользователя, 
** это будет расцениваться как частое и обычно неверное, ибо отправлено 
** сообщение большому количеству людей (подобно WALL).
** =====================================================================
*/

void Command::
cmd_kill(IRC& irc, int fd)
{
	string sender = (!_prefix.empty()) ? _prefix : irc.get_server(fd)->getName();
	vector<User*> users = irc.get_users();

	if (!irc.is_server(fd))					// если сообщение KILL от пользователя
	{
		int pos = irc.find_fd(users, fd);
		if (pos < 0)						// неверный отправитель
		{
			irc.push_cmd_queue(fd, irc.response(ERR_NOSUCHNICK, fd, sender, ERR_NOSUCHNICK_MESS));
			return;
		}
		if (!users[pos]->getMode('o'))		// у пользователя нет прав оператора сервера
		{
			irc.push_cmd_queue(fd, irc.response(ERR_NOPRIVILEGES, fd, sender, ERR_NOPRIVILEGES_MESS));
			return;
		}
	}

	if (_arguments.size() != 2)
	{
		irc.push_cmd_queue(fd, irc.response(ERR_NEEDMOREPARAMS, fd, _command, ERR_NEEDMOREPARAMS_MESS));
		return;
	}

	int pos = irc.find_name(users, _arguments[0]);
	if (pos < 0)
	{
		irc.push_cmd_queue(fd, irc.response(ERR_NOSUCHNICK, fd, sender, ERR_NOSUCHNICK_MESS));
		return;
	}
	
	irc.delete_user_from_channels(users[pos], " QUIT :" + _arguments[1]);
	irc.close_connection(users[pos]);	
}
