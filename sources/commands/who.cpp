#include "main.hpp"
#include "Class.Command.hpp"

/*
** ====================================================================
** Команда: WHO 
** Параметры: [<mask> ["o"]]
** ====================================================================
** Сообщение WHO используется клиентом для создания запроса, который 
** возвращает список информации, которая совпадает с параметром <mask>. 
** В отсутствии параметра <mask>, все видимые 
** (пользователи, которых не видно (пользовательский режим +i) и те, кто 
** находятся на других каналах, нежели запрашивающий клиент) попадают в 
** список. Результат может быть достигнут использованием вместо <name> "0" 
** или других масок, которые будут подставлять все возможные окончания. 
**
** <name> обратиться к WHO, подставленному против пользовательского 
** хоста, сервера, реального имени или никнейма, если канал <name> не найден. 
**
** Если параметр "o" прошел только операторам, обеспечивается возврат 
** только маски имени.  
** ====================================================================
*/

int Command::
cmd_who(IRC& irc, int fd)
{
	if (arguments.empty() || arguments[0] == "0" || arguments[0] == "*")
	{
		vector<User*> users = irc.get_users();
		for (size_t i = 0; i < users.size(); i++)
		{
			if (!users[i]->is_i_mode())
				irc.push_cmd_queue(fd, irc.response(RPL_WHOREPLY, fd, who_message(users[i]), ""));
		}
		irc.push_cmd_queue(fd, irc.response(RPL_ENDOFWHO, fd, "*", RPL_ENDOFWHO_MESS));
	}
	else if (arguments[0][0] == '&' || arguments[0][0] == '#')
	{
		string channel_name = arguments[0].substr(1);

		size_t n = (arguments[0][0] == '&')
					? irc.get_local_channels().count(channel_name)
					: irc.get_shared_channels().count(channel_name);
		if (n > 0)
		{
			vector<User*> users = (arguments[0][0] == '&')
									? irc.get_local_channels()[channel_name].get_users()
									: irc.get_shared_channels()[channel_name].get_users();
			for (size_t i = 0; i < users.size(); i++)
			{
				if (!users[i]->is_i_mode())
					irc.push_cmd_queue(fd, irc.response(RPL_WHOREPLY, fd, who_message(users[i]), ""));
			}
			irc.push_cmd_queue(fd, irc.response(RPL_ENDOFWHO, fd, arguments[0], RPL_ENDOFWHO_MESS));
		}
	}
	else
	{
		int i = IRC::find_name(irc.get_users(), arguments[0]);
		if (i >= 0)
			irc.push_cmd_queue(fd, irc.response(RPL_WHOREPLY, fd, who_message(irc.get_users()[i]), ""));
		else
		{
			irc.push_cmd_queue(fd, irc.response(ERR_NOSUCHSERVER, fd, irc.get_nickname(fd), ERR_NOSUCHSERVER_MESS));
			return 1;
		}
		
		irc.push_cmd_queue(fd, irc.response(RPL_ENDOFWHO, fd, irc.get_nickname(fd), RPL_ENDOFWHO_MESS));
	}
	return 0;
}

string Command::
who_message(const User& user)
{
	return "* "
			+ user.getUsername() + " "
			+ user.getHostname() + " "
			+ user.getServername() + " "
			+ user.getNickname() + " "
			+ "H" + (user.is_o_mode() ? "* " : " ")
			+ ":" + utils::int_to_str(user.getHopcount())
			+ user.getRealname();
}
