#include "main.hpp"
#include "Class.Command.hpp"

/*
** ====================================================================
** Команда: WHO 
** ====================================================================
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

void Command::
cmd_who(IRC& irc, int fd)
{
	if (_arguments.empty() || _arguments[0] == "0" || _arguments[0] == "*")
	{
		vector<User*> users = irc.get_users();
		for (size_t i = 0; i < users.size(); i++)
		{
			string prefix = "H";

			if (irc.is_server_operator(users[i]))
				prefix += "*";

			if (!users[i]->is_i_mode())
				irc.push_cmd_queue(fd, irc.response(RPL_WHOREPLY, fd, who_message(users[i], "*", prefix), ""));
		}
		irc.push_cmd_queue(fd, irc.response(RPL_ENDOFWHO, fd, "*", RPL_ENDOFWHO_MESS));
	}
	else if (_arguments[0][0] == '&' || _arguments[0][0] == '#')
	{
		string channel_name = _arguments[0];

		size_t n = irc.get_channels().count(channel_name);
		
		if (n > 0)
		{
			const Channel& channel = irc.get_channels()[channel_name];
			user_map users = irc.get_channels()[channel_name].get_users();

			for (user_iterator it = users.begin(); it != users.end(); it++)
			{
				if (!(it->first->is_i_mode()))
				{
					string prefix = "H";

					if (irc.is_server_operator(it->first))
						prefix += "*";
					if (channel.is_channel_operator(it->first))
						prefix += "@";
					irc.push_cmd_queue(fd, irc.response(RPL_WHOREPLY, fd, who_message(it->first, _arguments[0], prefix), ""));
				}
			}
			irc.push_cmd_queue(fd, irc.response(RPL_ENDOFWHO, fd, _arguments[0], RPL_ENDOFWHO_MESS));
		}
	}
	else
	{
		int i = IRC::find_name(irc.get_users(), _arguments[0]);
		if (i >= 0)
		{
			string prefix = "H";

			if (irc.is_server_operator(irc.get_users()[i]))
				prefix += "*";

			irc.push_cmd_queue(fd, irc.response(RPL_WHOREPLY, fd, who_message(irc.get_users()[i], _arguments[0], prefix), ""));
		}
		else
		{
			irc.push_cmd_queue(fd, irc.response(ERR_NOSUCHSERVER, fd, irc.get_nickname(fd), ERR_NOSUCHSERVER_MESS));
			return;
		}
		
		irc.push_cmd_queue(fd, irc.response(RPL_ENDOFWHO, fd, irc.get_nickname(fd), RPL_ENDOFWHO_MESS));
	}
}

string Command::
who_message(const User* user, string channel, string prefix)
{
	return channel + " "
			+ user->getUsername() + " "
			+ user->getHostname() + " "
			+ user->getServername() + " "
			+ user->getNickname() + " "
			+ prefix + " "
			+ ":" + utils::int_to_str(user->getHopcount()) + " "
			+ user->getRealname();
}
