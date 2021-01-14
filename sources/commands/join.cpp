#include "main.hpp"
#include "Class.Command.hpp"

/*
** ====================================================================
** Команда: JOIN <channel>{,<channel>} [<key>{,<key>}] 
** ====================================================================
** Команда JOIN используется клиентом для входа на канал. Так или иначе, 
** клиенту позволительно войти на канал, проверенным только сервером, к 
** которому подсоединен; все остальные серверы автоматически добавляют 
** пользователя на канал, когда получают уведомление от других серверов. 
** Условия выполнения команды: 
**		1.  Пользователь может быть приглашен, если канал invite-only; 
**		2.  Никнейм/имя пользователя/имя хоста не должны быть 
**  		забанеными; 
**		3.  Пароль (если установлен) должен быть верным. 
** ====================================================================
*/

int Command::cmd_join(IRC& irc, int fd)
{
	if (arguments.empty())
	{
		irc.push_cmd_queue(fd, irc.response_to_client(ERR_NEEDMOREPARAMS, fd, command, ERR_NEEDMOREPARAMS_MESS));
		return 1;
	}

	vector<string> channels = utils::split(arguments[0], ',');
	vector<string> keys = (arguments.size() > 1) ? utils::split(arguments[1], ',') : vector<string>();
	
	for (size_t i = 0; i < channels.size(); i++)
	{
		if (!(channels[i][0] == '&' || channels[i][0] == '#'))
		{
			irc.push_cmd_queue(fd, irc.response_to_client(ERR_NOSUCHCHANNEL, fd, channels[i], ERR_NOSUCHCHANNEL_MESS));
			continue;
		}
		
		string key = (i < keys.size()) ? keys[i] : string();
		string nickname = (prefix.size()) ? prefix : irc.get_user(fd)->getName();

		join_channel(irc, channels[i].substr(1), key, channels[i][0], nickname, fd); // channels[i].substr(1) - channel name, channels[i][0] - '&' or '#', prefix - user nickname
	}
	return 0;
}

void Command::
join_channel(IRC& irc,
			const string& channel_name,
			const string& channel_key,
			char channel_type,
			const string& nickname,
			int fd)
{
	int i;
	if ((i = IRC::find_fd(irc.get_servers(), fd)) >= 0)			// check if join recieved by server 
	{
		if (join_from_server(irc, channel_name, nickname))
			return;
		
		int i = IRC::find_name(irc.get_users(), nickname);
		string message = irc.full_name(irc.get_users()[i]) + " JOIN :" + channel_type + channel_name;

		irc.forward_message_to_servers(fd, message, true);
		irc.forward_message_to_clients(fd, message);
		return;
	}

	if (!Channel::is_valid_channel_name(channel_name))			// check channel name
	{
		irc.push_cmd_queue(fd, irc.response_to_client(ERR_NOSUCHCHANNEL, fd, channel_name, ERR_NOSUCHCHANNEL_MESS));
		return;
	}

	map<string, Channel>& channels = (channel_type == '&')
									? irc.get_local_channels()
									: irc.get_shared_channels();

	User* new_user = irc.get_user(fd);
	string message = irc.full_name(new_user) + " JOIN :" + channel_type + channel_name;

	// map<string, Channel>::iterator it = channels.find(channel_name);
	
	// if (it == channels.end())									// add new channel
	// {
	// 	channels.insert(make_pair(channel_name, Channel(channel_name, channel_key, new_user)));
	// 	map<string, Channel>::iterator it2 = channels.find(channel_name);

	// 	it2->second.add_operator(new_user);
	// 	it2->second.add_user(new_user);
	// 	irc.push_cmd_queue(fd, irc.full_name(new_user) + " JOIN :" + channel_type + channel_name + "\r\n");
	// 	irc.forward_message_to_servers(fd, message, true);
	// 	// отправить всем серверам, что создан канал	TODO
	// }
	if (!channels.count(channel_name))
	{
		channels.insert(make_pair(channel_name, Channel(channel_name, channel_key, new_user)));
		channels[channel_name].add_operator(new_user);
		channels[channel_name].add_user(new_user);
		irc.push_cmd_queue(fd, irc.full_name(new_user) + " JOIN :" + channel_type + channel_name + "\r\n");
		irc.forward_message_to_servers(fd, message, true);
	}
	else														// channel already exist
	{
		if (channels[channel_name].is_user_in_channel(nickname))			// check if user already in channel
			return;

		if (channels[channel_name].is_banned(nickname))						// check if user is banned 
		{
			irc.push_cmd_queue(fd, irc.response_to_client(ERR_BANNEDFROMCHAN, fd, "#" + channel_name, ERR_BANNEDFROMCHAN_MESS));
			return;
		}

		if (!channels[channel_name].is_valid_key(channel_key))				// check channel password
		{
			irc.push_cmd_queue(fd, irc.response_to_client(ERR_BADCHANNELKEY, fd, "#" + channel_name, ERR_BADCHANNELKEY_MESS));
			return;
		}

		if (channels[channel_name].is_invite_only())						// check if invite only channel
		{
			irc.push_cmd_queue(fd, irc.response_to_client(ERR_INVITEONLYCHAN, fd, "#" + channel_name, ERR_INVITEONLYCHAN_MESS));
			return;
		}
		// if (it->second.is_user_in_channel(nickname))			// check if user already in channel
		// 	return;

		// if (it->second.is_banned(nickname))						// check if user is banned 
		// {
		// 	irc.push_cmd_queue(fd, irc.response_to_client(ERR_BANNEDFROMCHAN, fd, "#" + channel_name, ERR_BANNEDFROMCHAN_MESS));
		// 	return;
		// }

		// if (!it->second.is_valid_key(channel_key))				// check channel password
		// {
		// 	irc.push_cmd_queue(fd, irc.response_to_client(ERR_BADCHANNELKEY, fd, "#" + channel_name, ERR_BADCHANNELKEY_MESS));
		// 	return;
		// }

		// if (it->second.is_invite_only())						// check if invite only channel
		// {
		// 	irc.push_cmd_queue(fd, irc.response_to_client(ERR_INVITEONLYCHAN, fd, "#" + channel_name, ERR_INVITEONLYCHAN_MESS));
		// 	return;
		// }

		channels[channel_name].add_user(new_user);
		// map<string, Channel>::iterator it = channels.find(channel_name);
		// it->second.add_user(new_user);

		irc.push_cmd_queue(fd, message + "\r\n");
		irc.forward_message_to_servers(fd, message, true);
		irc.forward_message_to_clients(fd, message);
		// send_topic(irc, fd, channel_type + channel_name, it->second.get_topic());			// отправляем Топик
		send_topic(irc, fd, channel_type + channel_name, channels[channel_name].get_topic());	// отправляем Топик
		// :irc.atw-inter.net 332 porrring #q1 ::New topic set
	}
	new_user->inc_channel_count();								// увеличиваем количество каналов, в которых состоит пользователь
	
	send_channel_users(irc, fd, channel_type, channel_name);	// отсылаем подключившемуся список всех пользователей канала
	irc.push_cmd_queue(fd, irc.response_to_client(RPL_ENDOFNAMES, fd, channel_type + channel_name, RPL_ENDOFNAMES_MESS));
	
	irc.print_channels(); //DEBUG
	
	return;
}

int Command::
join_from_server(IRC& irc, const string& channel_name, const string& nickname)
{
	int i = IRC::find_name(irc.get_users(), nickname);
	if (i < 0) return 1;
	User* new_user = irc.get_users()[i];

	map<string, Channel>& channels = irc.get_shared_channels();

	if (channels.count(channel_name))
		channels[channel_name].add_user(new_user);
	else
	{
		channels.insert(std::make_pair(channel_name, Channel(channel_name)));
		channels[channel_name].add_user(new_user);
	}
	return 0;
}
