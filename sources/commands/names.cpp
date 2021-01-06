#include "main.hpp"
#include "Class.Command.hpp"

/*
** ====================================================================
** Команда: NAMES 
** Параметры: [<channel>{,<channel>}]
** ====================================================================
** Используя команду NAMES, пользователь может получитт список всех 
** никнеймов, которые он видит на любом канале, на которых они находятся. 
** Имена каналов, которые они могут видеть это те, которые не приватные 
** (+p) или секретные (+s), или те, на которых сидит пользователь. 
** Параметр <channel> указывает, с какого канала(ов) собрать информацию. 
** Эта команда не возвращает кода ошибки из-за неправильных названий 
** каналов. 

** Если параметр <channel> не задан, выводится список всех каналов и имен 
** тех, кто на них находится. И к концу списка - список пользователей, 
** которые видимые, но не находятся ни на одной канале, или не на одном 
** видимом канале, которые начинаюся как 'channel' "*".
** ====================================================================
*/

int Command::
cmd_names(IRC& irc, int fd)
{
	vector<string> args = utils::split(arguments[0], ',');

	if (arguments.empty() || args[0] == "0")
	{
		// отправить всех видимых пользователей(?)
		return 0;
	}
	
	for (size_t i = 0; i < args.size(); i++)
	{
		if ((args[i][0] == '&' || args[i][0] == '#')
			&& is_channel_visible(irc, fd, args[i][0], args[i].substr(1)))
			send_channel_users(irc, fd, args[i][0], args[i].substr(1));
		else
			irc.push_cmd_queue(fd, irc.response_to_client(RPL_ENDOFNAMES, fd, args[i], RPL_ENDOFNAMES_MESS));
	}
	return 0;
}

void Command::
send_channel_users(IRC& irc, int fd, char channel_type, string channel_name)
{
	if ((channel_type == '&'
					? irc.get_local_channels().count(channel_name)
					: irc.get_shared_channels().count(channel_name)) == 0)
	{
		irc.push_cmd_queue(fd, irc.response_to_client(RPL_ENDOFNAMES, fd, channel_type + channel_name, RPL_ENDOFNAMES_MESS));
		return;
	}

	int i = IRC::find_fd(irc.get_users(), fd);		// ищем никнейм пользователя по его fd
	User* user = irc.get_users()[i];				// ????? МОЖЕТ НУЖНА ПРОВЕРКА
	
	Channel& channel = (channel_type == '&'
						? irc.get_local_channels().find(channel_name)->second
						: irc.get_shared_channels().find(channel_name)->second);

	string prefix = ":"
					+ irc.get_server_name() + " "
					+ utils::int_to_str(RPL_NAMREPLY) + " "
					+ user->getNickname()
					+ " = "
					+ channel_type
					+ channel_name
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
	irc.push_cmd_queue(fd, irc.response_to_client(RPL_ENDOFNAMES, fd, channel_type + channel_name, RPL_ENDOFNAMES_MESS));
}