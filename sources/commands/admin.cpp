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

int Command::cmd_admin(IRC& irc, int fd)
{
	if (arguments.size() > 1)
		return irc.push_mess_client(fd, ERR_NEEDMOREPARAMS);

	User* usr = irc.get_user(fd);

	if (!irc.is_server(fd) && arguments.empty())
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
	else if (!arguments.empty())
	{
		const vector<Server*>& servers = irc.get_servers();
		for (size_t i = 0; i < servers.size(); i++)
		{
			if (servers[i]->getName() == arguments[0])
			{
				irc.forward_to_servers(fd, ":" + usr->getNickname() + " ADMIN " + arguments[0] + "\r\n");
				return 0;	
			}
		}
		irc.push_mess_client(fd, ERR_NOSUCHSERVER);
	}

	return 1;
}
