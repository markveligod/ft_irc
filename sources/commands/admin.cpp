#include "Class.Command.hpp"
#include "Class.IRC.hpp"
#include "utils.hpp"

/*
** ====================================================================
** Команда: ADMIN
** ====================================================================
** Параметры: [message]
** ====================================================================
** Сообщение ADMIN используется для поиска администратора указанного 
** сервера, или текущего сервера, если параметр <server> не указан. Каждый 
** сервер должен иметь возможность отправить ADMIN-сообщения другим 
** серверам.  
** =====================================================================
*/

void Command::
cmd_admin(IRC& irc, int fd)
{
	string recepient = (!_prefix.empty()) ? _prefix : irc.get_user(fd)->getName();
	
	if (_arguments.size() > 1)
	{
		irc.push_cmd_queue(fd, irc.response(ERR_NEEDMOREPARAMS, recepient, _command, ERR_NEEDMOREPARAMS_MESS));
		return;
	}

	User* usr = irc.get_user(fd);

	if (_arguments.empty())
	{

		string admin_info = ":" + irc.get_server_name() + " " +
							utils::int_to_str(RPL_ADMINME) + " " +
							usr->getNickname() + " " +
							irc.get_server_name() +
							ADMINME + "\r\n" +

							":" + irc.get_server_name() + " " +
							utils::int_to_str(RPL_ADMINLOC1) + " " +
							usr->getNickname() +
							ADMINLOC1 + "\r\n" +

							":" + irc.get_server_name() + " " +
							utils::int_to_str(RPL_ADMINLOC2) + " " +
							usr->getNickname() +
							ADMINLOC2 + "\r\n" +

							":" + irc.get_server_name() + " " +
							utils::int_to_str(RPL_ADMINEMAIL) + " " +
							usr->getNickname() +
							ADMINEMAIL + "\r\n";
		
		irc.push_cmd_queue(fd, admin_info);
	}
	else
	{
		const vector<Server*>& servers = irc.get_servers();
		for (size_t i = 0; i < servers.size(); i++)
		{
			if (servers[i]->getName() == _arguments[0])
			{
				irc.forward_to_servers(fd, ":" + usr->getNickname() + " ADMIN " + _arguments[0] + "\r\n");
				return;	
			}
		}
		irc.push_cmd_queue(fd, irc.response(ERR_NOSUCHSERVER, recepient, _command, ERR_NOSUCHSERVER_MESS));
	}
}
