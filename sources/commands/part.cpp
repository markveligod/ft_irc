#include "main.hpp"
#include "Class.Command.hpp"

/*
** ====================================================================
** Команда: PART
** ====================================================================
** Параметры: <channel>{,<channel>} 
** ====================================================================
** Команда PART удаляет клиента, пославшего эту команду из списка 
** активных пользователей для всех каналов, указанных в параметре.
**
** PART #twilight_zone		; покинуть канал "#twilight_zone"
** PART #oz-ops,&group5		; покинуть каналы "&group5" и "#oz-ops". 
** ====================================================================
*/

void Command::
cmd_part(IRC& irc, int fd)
{
	if (_arguments.empty())
	{
		irc.push_cmd_queue(fd, irc.response(ERR_NEEDMOREPARAMS, fd, _command, ERR_NEEDMOREPARAMS_MESS));
		return;
	}

	vector<string> channels = utils::split(_arguments[0], ',');			// получаем вектор каналов

	channel_map& _channels = irc.get_channels();
	string exit_message = " :" + ((_arguments.size() == 2) ? _arguments[1] : string());

	// channels - список каналов, которые были в _arguments[0] перечислены через ','
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
		
		string message1 = (irc.fullname(user))
							   + " PART " + channel.getName() + message;
		string message2 =  ":" + user->getName()
							   + " PART " + channel.getName() + message;
		
		if (all_users.empty())
			irc.delete_channel(channel.getName());
		
		if (!irc.is_server(fd))
			irc.push_cmd_queue(fd, message1 + "\r\n");
		irc.forward_to_channel(fd, channel, message1);
		irc.forward_to_servers(fd, message2);
	}
	else
		irc.push_cmd_queue(fd, irc.response(RPL_ENDOFNAMES, fd, channel.getName(), RPL_ENDOFNAMES_MESS));
}
