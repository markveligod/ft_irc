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
		irc.push_cmd_queue(fd, irc.response(ERR_NEEDMOREPARAMS, fd, command, ERR_NEEDMOREPARAMS_MESS));
		return 1;
	}

	vector<string> channels = utils::split(arguments[0], ',');
	vector<string> keys = (arguments.size() > 1) ? utils::split(arguments[1], ',') : vector<string>();
	
	for (size_t i = 0; i < channels.size(); i++)
	{
		if (!(channels[i][0] == '&' || channels[i][0] == '#'))
		{
			irc.push_cmd_queue(fd, irc.response(ERR_NOSUCHCHANNEL, fd, channels[i], ERR_NOSUCHCHANNEL_MESS));
			continue;
		}
		
		string key = (i < keys.size()) ? keys[i] : string();

		User *user = (prefix.size()) ? irc.get_user(prefix) : irc.get_user(fd);
		if (!user)
			return 1;
		join_channel(irc, channels[i], key, user, fd);
	}
	return 0;
}

void Command::
join_channel(IRC& irc,
			const string& channel_name,
			const string& channel_key,
			User* user,
			int fd)
{
	if (is_server(irc, fd))											// check if join recieved by server 
	{
		join_from_server(irc, user, channel_name);

		string message1 = irc.full_name(user) + " JOIN :" + channel_name;
		string message2 = ":" + user->getName() + " JOIN :" + channel_name;

		irc.forward_message_to_clients(irc, message1);
		irc.forward_message_to_servers(fd, message2, true);
		return;
	}

	if (!Channel::is_valid_channel_name(channel_name))				// check channel name
	{
		irc.push_cmd_queue(fd, irc.response(ERR_NOSUCHCHANNEL, fd, channel_name, ERR_NOSUCHCHANNEL_MESS));
		return;
	}

	map<string, Channel>& channels = irc.get_channels();

	string message1 = irc.full_name(user) + " JOIN :" + channel_name;
	string message2 = ":" + user->getName() + " JOIN " + channel_name;

	if (!channels.count(channel_name))
	{
		channels.insert(make_pair(channel_name, Channel(channel_name, channel_key, user)));
		channels[channel_name].add_user(user);
		channels[channel_name].set_operator(user);
		irc.push_cmd_queue(fd, message1 + "\r\n");
		irc.push_cmd_queue(fd, irc.full_name(user) + " MODE :" + channel_name + " +o " + user->getName() + "\r\n");

		irc.forward_message_to_clients(irc, message1);
		irc.forward_message_to_servers(fd, message2, true);
	}
	else															// channel already exist
	{
		if (channels[channel_name].is_user_in_channel(user))		// check if user already in channel
			return;

		if (channels[channel_name].is_banned(user))					// check if user is banned 
		{
			irc.push_cmd_queue(fd, irc.response(ERR_BANNEDFROMCHAN, fd, channel_name, ERR_BANNEDFROMCHAN_MESS));
			return;
		}

		if (!channels[channel_name].is_valid_key(channel_key))		// check channel password
		{
			irc.push_cmd_queue(fd, irc.response(ERR_BADCHANNELKEY, fd, channel_name, ERR_BADCHANNELKEY_MESS));
			return;
		}

		if (channels[channel_name].is_invite_only())				// check if invite only channel
		{
			irc.push_cmd_queue(fd, irc.response(ERR_INVITEONLYCHAN, fd, channel_name, ERR_INVITEONLYCHAN_MESS));
			return;
		}

		channels[channel_name].add_user(user);

		irc.push_cmd_queue(fd, message1 + "\r\n");

		irc.forward_message_to_clients(irc, message1);
		irc.forward_message_to_servers(fd, message2, true);

		send_topic(irc, fd, channel_name, channels[channel_name].get_topic());	// отправляем Топик
	}
	user->inc_channel_count();									// увеличиваем количество каналов, в которых состоит пользователь
	
	send_channel_users(irc, fd, user, channels[channel_name]);			// отсылаем подключившемуся список всех пользователей канала
	irc.push_cmd_queue(fd, irc.response(RPL_ENDOFNAMES, fd, channel_name, RPL_ENDOFNAMES_MESS));
	
	//irc.print_channels(); //DEBUG
	
	return;
}

int Command::
join_from_server(IRC& irc, User* user, const string& channel_name)
{
	map<string, Channel>& channels = irc.get_channels();

	if (channels.count(channel_name))
		channels[channel_name].add_user(user);
	else
	{
		channels.insert(std::make_pair(channel_name, Channel(channel_name)));
		channels[channel_name].add_user(user);
		utils::print_line("Channel " + channel_name + " created");
	}
	utils::print_line("User " + user->getNickname() + " joined channel " + channel_name);
	std::cout << "DEBUG Channel members list:\n";
	channels[channel_name].print_users();
	user->inc_channel_count();
	return 0;
}
