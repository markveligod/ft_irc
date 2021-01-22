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

void Command::
cmd_topic(IRC& irc, int fd)
{
	if (_arguments.empty())
	{
		irc.push_cmd_queue(fd, irc.response(ERR_NEEDMOREPARAMS, fd, _command, ERR_NEEDMOREPARAMS_MESS));
		return;
	}

	vector<string> channels = utils::split(_arguments[0], ',');

	for (size_t i = 0; i < channels.size(); i++)		// channels[i] - (#/&)channel_name
	{
		Channel* chann = irc.get_channel(channels[i]);
		User* user = (_prefix.size()) ? irc.get_user(_prefix) : irc.get_user(fd);
        if (!user) return;

		if (!chann || !user								// channel doesn't exist
			|| !chann->is_user_in_channel(user))		// check, if user in channel
		{
			irc.push_cmd_queue(fd, irc.response(ERR_NOTONCHANNEL, fd, channels[i], ERR_NOTONCHANNEL_MESS));
		}
		else if (chann->is_topic_only_oper()
				&& !chann->is_channel_operator(user))				// check, if channel +t mode and user isn't operator
		{
			irc.push_cmd_queue(fd, irc.response(ERR_CHANOPRIVSNEEDED, fd, channels[i], ERR_CHANOPRIVSNEEDED_MESS));
		}
		else if (_arguments.size() == 1)
			send_topic(irc, fd, channels[i], chann->get_topic());
		else if (_arguments.size() > 2)
			irc.push_cmd_queue(fd, irc.response(ERR_NEEDMOREPARAMS, fd, channels[i], ERR_NEEDMOREPARAMS_MESS));
		else
		{
			chann->set_topic(_arguments[1]);

			string mess_to_user = 	irc.fullname(user) + " TOPIC " + chann->getName() + " :" + _arguments[1];
			string mess_to_server =	":" + user->getName() + " TOPIC " + chann->getName() + " :" + _arguments[1];

			irc.push_cmd_queue(fd, mess_to_user + "\r\n");
			irc.forward_to_channel(fd,*chann, mess_to_user);
			irc.forward_to_servers(fd, mess_to_server);
		}
	}
	return;
}

void Command::
send_topic(IRC& irc, int fd, const string& channel_name, const string& topic)
{
	if (topic.empty())
		irc.push_cmd_queue(fd, irc.response(RPL_NOTOPIC, fd, channel_name, RPL_NOTOPIC_MESS));
	else
		irc.push_cmd_queue(fd, irc.response(RPL_TOPIC, fd, channel_name, " :" + topic));
}
