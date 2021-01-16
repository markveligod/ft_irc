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
	vector<string> args = (!arguments.empty()) ? utils::split(arguments[0], ',') : vector<string>();

	User* user = (prefix.size()) ? irc.get_user(prefix) : irc.get_user(fd);
	if (!user) return 1;

	if (arguments.empty() || args[0] == "0" || atoi(args[0].c_str()) == irc.get_localhost_port())
	{
		map<string, Channel>& channels = irc.get_channels();

		if (!is_server(irc, fd))											// если fd принадлежит Серверу, то ему не надо отправлять список локальных каналов 
		{
			for (map<string, Channel>::iterator it = channels.begin(); it != channels.end(); it++)
			{
				if (it->second.is_visible() && it->second.is_local_channel())
					send_channel_users(irc, fd, user, it->second);
			}
		}

		for (map<string, Channel>::iterator it = channels.begin(); it != channels.end(); it++)
		{
			if (it->second.is_visible() && it->second.is_network_channel())
					send_channel_users(irc, fd, user, it->second);
		}

		send_users_without_channel(irc, fd);
		irc.push_cmd_queue(fd, irc.response(RPL_ENDOFNAMES, fd, "*", RPL_ENDOFNAMES_MESS));
		return 0;
	}
	
	map<string, Channel> channels = irc.get_channels();

	for (size_t i = 0; i < args.size(); i++)
	{
		if (channels.count(args[i]))
		{
			if (channels[args[i]].is_visible())
			{
				send_channel_users(irc, fd, user, channels[args[i]]);
				irc.push_cmd_queue(fd, irc.response(RPL_ENDOFNAMES, fd, args[i], RPL_ENDOFNAMES_MESS));
			}
		}
		else
			irc.push_cmd_queue(fd, irc.response(RPL_ENDOFNAMES, fd, args[i], RPL_ENDOFNAMES_MESS));
	}
	return 0;
}

void Command::
send_channel_users(IRC& irc, int fd, User* user, Channel& channel)
{
	map<User*, ModeUser>& users = channel.get_users();
	map<User *, ModeUser>::const_iterator it = users.begin();

	string prefix = ":"
					+ irc.get_server_name() + " "
					+ utils::int_to_str(RPL_NAMREPLY) + " "
					+ user->getName()
					+ " = "
					+ channel.getName()
					+ " :";

	while (it != users.end())
	{
		string response = prefix;

		bool first = true;
		while (it != users.end())
		{
			user = (it->first);
			string nickname = user->getName();

			if (channel.is_operator(user))
				nickname = "@" + nickname;
			if (!channel.is_operator(user) && channel.is_have_voice(user))
				nickname = "+" + nickname;
			if (!first)
				nickname = " " + nickname;
			first = false;
			if (response.size() + nickname.size() < MAX_MESSAGE_LEN)
			{
				response += nickname;
				it++;
				first = false;
			}
			else
				break;
		}
		irc.push_cmd_queue(fd, response + "\r\n");
		if (first)
			it++;
	}
}

void Command::
send_users_without_channel(IRC& irc, int fd)
{
	std::vector<User*>& users = irc.get_users();
	int i = IRC::find_fd(users, fd);

	string prefix = ":"
					+ irc.get_server_name() + " "
					+ utils::int_to_str(RPL_NAMREPLY) + " "
					+ users[i]->getName()
					+ " * * :";

	for (size_t i = 0; i < users.size(); )
	{
		string response = prefix;
	
		bool first = true;
		while (i < users.size())
		{
			if (!users[i]->getChannelCount())
			{
				string nickname = users[i]->getName();
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
			else
				i++;
		}
		if (response != prefix)
			irc.push_cmd_queue(fd, response + "\r\n");
	}
}