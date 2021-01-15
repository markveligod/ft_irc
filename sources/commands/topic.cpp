#include "Class.Command.hpp"
#include "Class.IRC.hpp"

/*
** ====================================================================
** Команда: TOPIC 
** ====================================================================
** Параметры: <channel> [<topic>] 
** ====================================================================
** Используется для изменения или просмотра топика канала. Топик канала 
** <channel> останется прежним, если не будет дан новый топик <topic>. 
** Если параметр <topic> подставлен - топик канала изменится, если режим 
** канала позволяет это сделать.
** =====================================================================
*/

int Command::
cmd_topic(IRC& irc, int fd)
{
	if (arguments.empty())
	{
		irc.push_cmd_queue(fd, irc.response_to_client(ERR_NEEDMOREPARAMS, fd, command, ERR_NEEDMOREPARAMS_MESS));
		return 1;
	}

	vector<string> channels = utils::split(arguments[0], ',');

	for (size_t i = 0; i < channels.size(); i++)		// channels[i] - (#/&)channel_name
	{
		Channel* chann = irc.get_channel(channels[i]);

		if (!chann										// channel doesn't exist
			|| !chann->is_user_in_channel(fd))			// check, if user in channel
		{
			irc.push_cmd_queue(fd, irc.response_to_client(ERR_NOTONCHANNEL, fd, channels[i], ERR_NOTONCHANNEL_MESS));
		}
		else if (chann->is_topic_only_oper()
				&& !chann->is_operator(fd))				// check, if channel +t mode and user isn't operator
		{
			irc.push_cmd_queue(fd, irc.response_to_client(ERR_CHANOPRIVSNEEDED, fd, channels[i], ERR_CHANOPRIVSNEEDED_MESS));
		}
		else if (arguments.size() == 1)
			send_topic(irc, fd, channels[i], chann->get_topic());
		else
			chann->set_topic(arguments[1]);
	}
	return 0;
}

void Command::
send_topic(IRC& irc, int fd, const string& channel_name, const string& topic)
{
	if (topic.empty())
		irc.push_cmd_queue(fd, irc.response_to_client(RPL_NOTOPIC, fd, channel_name, RPL_NOTOPIC_MESS));
	else
		irc.push_cmd_queue(fd, irc.response_to_client(RPL_TOPIC, fd, channel_name, " :" + topic));
}
