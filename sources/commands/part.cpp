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
		irc.push_cmd_queue(fd, irc.response_to_client(ERR_NEEDMOREPARAMS, fd, command, ERR_NEEDMOREPARAMS_MESS));
		return 1;
	}

	vector<string> channels = utils::split(arguments[0], ',');			// получаем вектор каналов

	map<string, Channel>& local_channels = irc.get_local_channels();
	map<string, Channel>& shared_channels = irc.get_shared_channels();

	string exit_message = (arguments.size() == 2) ? (" :" + arguments[1]) : " :Left the channel";

	// channels - список каналов, которые были в arguments[0] перечислены через ','
	// channels[i].substr(1) - название канала без первого символа '&' или '#'

	for (size_t i = 0; i < channels.size(); i++)
	{
		if (channels[i][0] == '&')
		{
			map<string, Channel>::iterator it = local_channels.find(channels[i].substr(1));
			if (it != local_channels.end())
				leave_channel(irc, it->second, '&', fd, exit_message);
			else
				irc.push_cmd_queue(fd, irc.response_to_client(ERR_NOSUCHCHANNEL, fd, channels[i], ERR_NOSUCHCHANNEL_MESS));
		}
		else if (channels[i][0] == '#')
		{
			map<string, Channel>::iterator it = shared_channels.find(channels[i].substr(1));
			if (it != shared_channels.end())
				leave_channel(irc, it->second, '#', fd, exit_message);
			else
				irc.push_cmd_queue(fd, irc.response_to_client(ERR_NOSUCHCHANNEL, fd, channels[i], ERR_NOSUCHCHANNEL_MESS));
		}
		else
			irc.push_cmd_queue(fd, irc.response_to_client(ERR_NOSUCHCHANNEL, fd, channels[i], ERR_NOSUCHCHANNEL_MESS));
	}

	return 0;
}

/*
** В leave_channel передаем канал, из которого хотим выйти
** и fd клиента, который хочет выйти
*/
void Command::
leave_channel(IRC& irc, Channel& channel, char type, int fd, string message)
{
	vector<User*>& users = channel.get_users();
	vector<User*>& operators = channel.get_operators();


	int i = IRC::find_fd(users, fd);
	if (i >= 0)
	{
		users[i]->dec_channel_count();
		users.erase(users.begin() + i);
		
		string full_message = irc.full_name(users[i]) + " PART " + type + channel.getName() + message;
		irc.push_cmd_queue(fd, full_message + "\r\n");

		i = IRC::find_fd(operators, fd);
		if (i >= 0)
			operators.erase(operators.begin() + i);
		
		if (users.empty())
			irc.delete_channel(channel.getName(), type);
		
		irc.forward_message_to_servers(fd, full_message, true);
		irc.forward_message_to_clients(fd, full_message);
	}
	else
		irc.push_cmd_queue(fd, irc.response_to_client(RPL_ENDOFNAMES, fd, type + channel.getName(), RPL_ENDOFNAMES_MESS));
}