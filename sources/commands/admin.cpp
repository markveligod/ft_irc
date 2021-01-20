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
	if (!irc.is_server(fd))
	{
		string admin_info = ":" + irc.get_server_name() + " " +
							utils::int_to_str(RPL_ADMINME) + " " +
							irc.get_user(fd)->getNickname() + " " +
							irc.get_server_name() +
							ADMINME + "\r\n" +

							":" + irc.get_server_name() + " " +
							utils::int_to_str(RPL_ADMINLOC1) + " " +
							irc.get_user(fd)->getNickname() + " " +
							ADMINLOC1 + "\r\n" +

							":" + irc.get_server_name() + " " +
							utils::int_to_str(RPL_ADMINLOC2) + " " +
							irc.get_user(fd)->getNickname() + " " +
							ADMINLOC2 + "\r\n" +

							":" + irc.get_server_name() + " " +
							utils::int_to_str(RPL_ADMINEMAIL) + " " +
							irc.get_user(fd)->getNickname() + " " +
							ADMINEMAIL + "\r\n";
	}

	return 0;
}
