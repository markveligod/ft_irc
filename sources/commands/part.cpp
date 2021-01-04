#include "main.hpp"
#include "Class.Command.hpp"

using std::string;
using std::vector;
using std::map;

/*
** ====================================================================
** Команда: PART <channel>{,<channel>} 
** ====================================================================
** Сообщение PART удаляет клиента, пославшего эту команду из списка 
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

	string exit_message = (arguments.size() == 2) ? (": " + arguments[1]) : ": Left the channel";
	std::cout << "DEBUG EXIT MESSAGE" << exit_message << std::endl;

	// (void)fd; (void)irc;
	// std::cout << "PREFIX: " << prefix << "|\n";
	// std::cout << "COMMAND: " << command << "|\n";
	// for (size_t i = 0; i < arguments.size(); i++)
	// 	std::cout << "ARG: " << arguments[i] << std::endl;
	// if (arguments.empty())
	// 	std::cout << "ARG: " << "NOTHING" << std::endl;


	// channels - список каналов, которые были в arguments[0] перечислены через ','
	// channels[i].substr(1) - название канала без первого символа '&' или '#'

	for (size_t i = 0; i < channels.size(); i++)
	{
		if (channels[i][0] == '&')
		{
			map<std::string, Channel>::iterator it = local_channels.find(channels[i].substr(1));
			if (it != local_channels.end())
				leave_channel(irc, it->second, '&', fd, exit_message);
			else
				irc.push_cmd_queue(fd, irc.response_to_client(ERR_NOSUCHCHANNEL, fd, channels[i], ERR_NOSUCHCHANNEL_MESS));
		}
		else if (channels[i][0] == '#')
		{
			map<std::string, Channel>::iterator it = shared_channels.find(channels[i].substr(1));
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

	int index = IRC::find_fd(users, fd);
	if (index >= 0)
	{
		users.erase(users.begin() + index);
		irc.push_cmd_queue(fd, irc.response_to_client(RPL_LEAVE_CHANNEL, fd, type + channel.get_name(), message));

		index = IRC::find_fd(operators, fd);
		if (index >= 0)
			operators.erase(operators.begin() + index);
		
		if (users.empty())
			irc.delete_channel(channel.get_name(), type);
	}
	else
		irc.push_cmd_queue(fd, irc.response_to_client(ERR_NOTONCHANNEL, fd, type + channel.get_name(), ERR_NOTONCHANNEL_MESS));
}