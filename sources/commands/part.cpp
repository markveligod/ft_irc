#include "main.hpp"
#include "Class.Command.hpp"

/*
** ====================================================================
** Команда: PART <channel>{,<channel>} 
** ====================================================================
** Команда PART удаляет клиента, пославшего эту команду из списка 
** активных пользователей для всех каналов, указанных в параметре.
**
** PART #twilight_zone		; покинуть канал "#twilight_zone"
** PART #oz-ops,&group5		; покинуть каналы "&group5" и "#oz-ops". 
** ====================================================================
*/

int Command::cmd_part(IRC& irc, int fd)
{
	if (arguments.empty())
	{
		irc.push_cmd_queue(fd, irc.response(ERR_NEEDMOREPARAMS, fd, command, ERR_NEEDMOREPARAMS_MESS));
		return 1;
	}

	vector<string> channels = utils::split(arguments[0], ',');			// получаем вектор каналов

	channel_map& _channels = irc.get_channels();
	string exit_message = " :" + ((arguments.size() == 2) ? arguments[1] : string());

	// channels - список каналов, которые были в arguments[0] перечислены через ','
	// channels[i].substr(1) - название канала без первого символа ('&' или '#')

	for (size_t i = 0; i < channels.size(); i++)
	{
		char type = channels[i][0];

		if (type == '&' || type == '#')
		{
			channel_map::iterator it = _channels.find(channels[i]);
			if (it != _channels.end())
				leave_channel(irc, it->second, fd, exit_message);
			else
				irc.push_cmd_queue(fd, irc.response(ERR_NOSUCHCHANNEL, fd, channels[i], ERR_NOSUCHCHANNEL_MESS));
		}
		else
			irc.push_cmd_queue(fd, irc.response(ERR_NOSUCHCHANNEL, fd, channels[i], ERR_NOSUCHCHANNEL_MESS));
	}

	return 0;
}

/*
** В leave_channel передаем канал, из которого хотим выйти
** и fd клиента, который хочет выйти
*/
void Command::
leave_channel(IRC& irc, Channel& channel, int fd, string message)
{
	user_map& all_users = channel.get_users();
	User* user = find_user(all_users, fd);
	if (user)
	{
		user->dec_channel_count();
		all_users.erase(user);
		
		string full_message = ((is_server(irc, fd)) 
									? ":" + prefix
									: irc.full_name(user))
								+ " PART " + channel.getName() + message;
		irc.push_cmd_queue(fd, full_message + "\r\n");
		
		if (all_users.empty())
			irc.delete_channel(channel.getName());
		
		irc.forward_message_to_servers(fd, full_message, true);
		irc.forward_message_to_clients(irc, full_message);
	}
	else
		irc.push_cmd_queue(fd, irc.response(RPL_ENDOFNAMES, fd, channel.getName(), RPL_ENDOFNAMES_MESS));
}
