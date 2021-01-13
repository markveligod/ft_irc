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
		string nickname = (prefix.size()) ? prefix : irc.get_user(fd)->getNickname();

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
	if (!Channel::is_valid_channel_name(channel_name))			// check channel name
	{
		irc.push_cmd_queue(fd, irc.response_to_client(ERR_NOSUCHCHANNEL, fd, channel_name, ERR_NOSUCHCHANNEL_MESS));
		return;
	}

	map<string, Channel>& channels = (channel_type == '&')
									? irc.get_local_channels()
									: irc.get_shared_channels();

	User* new_user = irc.get_user(fd);
	map<string, Channel>::iterator it = channels.find(channel_name);
	
	if (it == channels.end())									// add new channel
	{
		channels.insert(make_pair(channel_name, Channel(channel_name, channel_key, new_user, irc)));
		map<string, Channel>::iterator it2 = channels.find(channel_name);
		if (it2 == channels.end())
			std::cout << "DEBUG: hzzzzzz\n";
		std::cout << "DEBUG: channel name " << it2->first << std::endl;
		it2->second.add_operator(new_user);
		it2->second.add_user(new_user);
		irc.push_cmd_queue(fd, irc.full_name(new_user) + " JOIN :" + channel_type + channel_name + "\r\n");
		// отправить всем серверам, что создан канал	TODO
	}
	else														// channel already exist
	{
		if (it->second.is_user_in_channel(nickname))			// check if user already in channel
			return;

		if (it->second.is_banned(nickname))						// check if user is banned 
		{
			irc.push_cmd_queue(fd, irc.response_to_client(ERR_BANNEDFROMCHAN, fd, "#" + channel_name, ERR_BANNEDFROMCHAN_MESS));
			return;
		}

		if (!it->second.is_valid_key(channel_key))				// check channel password
		{
			irc.push_cmd_queue(fd, irc.response_to_client(ERR_BADCHANNELKEY, fd, "#" + channel_name, ERR_BADCHANNELKEY_MESS));
			return;
		}

		if (it->second.is_invite_only())						// check if invite only channel
		{
			irc.push_cmd_queue(fd, irc.response_to_client(ERR_INVITEONLYCHAN, fd, "#" + channel_name, ERR_INVITEONLYCHAN_MESS));
			return;
		}

		map<string, Channel>::iterator it = channels.find(channel_name);
		it->second.add_user(new_user);
		irc.push_cmd_queue(fd, irc.full_name(new_user) + " JOIN :" + channel_type + channel_name + "\r\n");
		send_topic(irc, fd, channel_type + channel_name, it->second.get_topic());			// отправляем Топик
		// :irc.atw-inter.net 332 porrring #q1 ::New topic set
	}
	new_user->inc_channel_count();								// увеличиваем количество каналов, в которых состоит пользователь
	
	send_channel_users(irc, fd, channel_type, channel_name);	// отсылаем подключившемуся список все пользователей канала
	irc.push_cmd_queue(fd, irc.response_to_client(RPL_ENDOFNAMES, fd, channel_type + channel_name, RPL_ENDOFNAMES_MESS));
	
	irc.print_channels(); //DEBUG
	
	return;
}