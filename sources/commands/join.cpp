#include "main.hpp"
#include "Class.Command.hpp"

/*
** ====================================================================
** Команда: JOIN
** ====================================================================
** Параметры: <channel>{,<channel>} [<key>{,<key>}]
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

int Command::
cmd_join(IRC& irc, int fd)
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
		if (!(channels[i][0] == '&' || channels[i][0] == '#') || !Channel::is_valid_channel_name(channels[i]))
		{
			irc.push_cmd_queue(fd, irc.response(ERR_NOSUCHCHANNEL, fd, channels[i], ERR_NOSUCHCHANNEL_MESS));
			continue;
		}
		
		string key = (i < keys.size()) ? keys[i] : string();

		User* user = (prefix.size()) ? irc.get_user(prefix) : irc.get_user(fd);
		if (!user) return 1;

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
	channel_map& channels = irc.get_channels();

	string mess_to_user = 		irc.full_name(user) + " JOIN :" + channel_name;
	string mess_to_server =		":" + user->getName() + " JOIN " + channel_name;
	string mode_message = 		":" + user->getName() + " MODE " + channel_name + " +o " + user->getName();

	if (!channels.count(channel_name))								// create new server
	{
		channels.insert(make_pair(channel_name, Channel(channel_name, channel_key, user)));
		channels[channel_name].add_user(user);

		if (!irc.is_server(fd))
			irc.push_cmd_queue(fd, mess_to_user + "\r\n");
		
		irc.forward_to_channel(fd, channel_name, mess_to_user);
		irc.forward_to_servers(fd, mess_to_server);
		
		if (!is_server(irc, fd))
		{
			channels[channel_name].set_operator(user);
			irc.forward_to_servers(fd, mode_message);
		}

		utils::print_line("Channel " + channel_name + " created");
		utils::print_line("User " + user->getNickname() + " joined channel " + channel_name);
	}
	else															// channel already exist
	{
		Channel& channel = channels[channel_name];

		if (channel.is_user_in_channel(user))		// check if user already in channel
			return;

		if (channel.is_banned(user))				// check if user is banned 
		{
			irc.push_cmd_queue(fd, irc.response(ERR_BANNEDFROMCHAN, fd, channel_name, ERR_BANNEDFROMCHAN_MESS));
			return;
		}

		if (!channel.is_valid_key(channel_key))		// check channel password
		{
			irc.push_cmd_queue(fd, irc.response(ERR_BADCHANNELKEY, fd, channel_name, ERR_BADCHANNELKEY_MESS));
			return;
		}

		if (channel.is_invite_only())				// check if invite only channel
		{
			irc.push_cmd_queue(fd, irc.response(ERR_INVITEONLYCHAN, fd, channel_name, ERR_INVITEONLYCHAN_MESS));
			return;
		}

		channel.add_user(user);
		utils::print_line("User " + user->getNickname() + " joined channel " + channel_name);

		if (!irc.is_server(fd))
		{
			irc.push_cmd_queue(fd, mess_to_user + "\r\n");
			if (channel.get_topic().size())
				send_topic(irc, fd, channel_name, channel.get_topic());	// отправляем Топик
		}
		irc.forward_to_channel(fd, channel_name, mess_to_user);
		irc.forward_to_servers(fd, mess_to_server);


		// std::cout << "DEBUG Channel members list:\n";
		// channels[channel_name].print_users();
	}
	user->inc_channel_count();											// увеличиваем количество каналов, в которых состоит пользователь
	
	if (!irc.is_server(fd))
	{
		send_channel_users(irc, fd, user, channels[channel_name]);		// отсылаем подключившемуся список всех пользователей канала
		irc.push_cmd_queue(fd, irc.response(RPL_ENDOFNAMES, fd, channel_name, RPL_ENDOFNAMES_MESS));
	}
	
	return;
}
