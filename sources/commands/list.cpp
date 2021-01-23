#include "Class.Command.hpp"
#include "Class.IRC.hpp"
#include "utils.hpp"

/*
** ====================================================================
** Команда: LIST
** ====================================================================
** Параметры: [<channel>{,<channel>}]
** ====================================================================
** LIST используется для вывода списка каналов и их топиков. Если 
** используется параметр <channel>, то выводится только статус этого 
** канала. Приватные каналы указаны (без их топиков) как каналы "Prv", без 
** указания количества клиентов, сидящих на этом канале. Само собой - 
** секретные каналы не приводятся в списке, 
** если, конечно, клиент не является членом подобного канала.
** =====================================================================
*/

void Command::
cmd_list(IRC& irc, int fd)
{
	User* usr = (!_prefix.empty()) ? irc.get_user(_prefix) : irc.get_user(fd);
	if (!usr) return;

	if (_arguments.size() > 1)
	{
		irc.push_cmd_queue(fd, irc.response(ERR_NEEDMOREPARAMS, usr->getName(), string(), ERR_NEEDMOREPARAMS_MESS));
		return;
	}

	string prefix = ":" + irc.get_server_name() + " ";
	string list_mess(512,0);

	list_mess = prefix +
				utils::int_to_str(RPL_LISTSTART) + " " +
				usr->getName() + " " +
				RPL_LISTSTART_MESS + "\r\n";
	irc.push_cmd_queue(fd, list_mess);

	channel_map& channels = irc.get_channels();

	if (_arguments.empty())
	{
		for (channel_iterator it = channels.begin(); it != channels.end(); it++)
		{
			if (!it->second.getModeChannel('s'))
			{
				list_mess = prefix +
						utils::int_to_str(RPL_LIST) + " " +
						usr->getName() + " " +
						it->first + " " +
						utils::int_to_str(it->second.get_users().size()) + " :" +
						it->second.get_topic() + "\r\n";
				irc.push_cmd_queue(fd, list_mess);
			}
		}
	}
	else
	{
		vector<string> input_channels = utils::split(_arguments[0], ',');
		channel_map all_channels = irc.get_channels();
		for (size_t i = 0; i < input_channels.size(); i++)
		{
			if (!all_channels.count(input_channels[i]))
				continue;
			if (!all_channels[input_channels[i]].getModeChannel('s'))
			{
				list_mess = prefix +
						utils::int_to_str(RPL_LIST) + " " +
						usr->getName() + " " +
						input_channels[i] + " " +
						utils::int_to_str(all_channels[input_channels[i]].get_users().size()) + " :" +
						all_channels[input_channels[i]].get_topic() + "\r\n";
				irc.push_cmd_queue(fd, list_mess);
			}
		}
	}

	list_mess = prefix +
				utils::int_to_str(RPL_LISTEND) + " " +
				usr->getName() + " " +
				RPL_LISTEND_MESS + "\r\n";
	irc.push_cmd_queue(fd, list_mess);
}
