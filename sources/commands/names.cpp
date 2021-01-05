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
cmd_names(IRC& irc, int fd)
{
	(void)irc; (void)fd;

	vector<string> args = utils::split(arguments[0], ',');

	if (arguments.empty() || args[0] == "0")
	{
		// отправить всех видимых пользователей
		return 0;
	}
	
	for (size_t i = 0; i < args.size(); i++)
	{
		if (args[i][0] == '&' || args[i][0] == '#')
			send_channel_users(irc, fd, args[i][0], args[i].substr(1));
		else
			irc.push_cmd_queue(fd, irc.response_to_client(RPL_ENDOFNAMES, fd, args[i], RPL_ENDOFNAMES_MESS));
	}
	return 0;
}

void Command::
send_channel_users(IRC& irc, int fd, char channel_type, string cnannel_name)
{
	if ((channel_type == '&'
					? irc.get_local_channels().count(cnannel_name)
					: irc.get_shared_channels().count(cnannel_name)) == 0)
	{
		irc.push_cmd_queue(fd, irc.response_to_client(RPL_ENDOFNAMES, fd, channel_type + cnannel_name, RPL_ENDOFNAMES_MESS));
		return;
	}

	int i = IRC::find_fd(irc.get_users(), fd);		// ищем никнейм пользователя по его fd
	User* user = irc.get_users()[i];				// ????? МОЖЕТ НУЖНА ПРОВЕРКА
	
	Channel& channel = (channel_type == '&'
						? irc.get_local_channels().find(cnannel_name)->second
						: irc.get_shared_channels().find(cnannel_name)->second);

	string prefix = ":"
					+ irc.get_server_name() + " "
					+ utils::int_to_str(RPL_NAMREPLY) + " "
					+ user->getNickname()
					+ " = "
					+ channel_type
					+ cnannel_name
					+ " :";
	const vector<User*>& users = channel.get_users();

	for (size_t i = 0; i < users.size(); )
	{
		string response = prefix;
	
		bool first = true;
		while (i < users.size())
		{
			string nickname = users[i]->getNickname();
			if (channel.is_operator(nickname))
				nickname = "@" + nickname;
			if (!first)
				nickname = " " + nickname;
			first = false;
			if (response.size() + nickname.size() < MAX_MESSAGE_LEN)
			{
				response += nickname;
				i++;
			} else
				break;
		}
		irc.push_cmd_queue(fd, response + "\r\n");
	}
	irc.push_cmd_queue(fd, irc.response_to_client(RPL_ENDOFNAMES, fd, channel_type + cnannel_name, RPL_ENDOFNAMES_MESS));
}