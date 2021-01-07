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
** Если параметр <topic> подставлен, - топик канала изменится, если режим 
** канала позволяет это сделать.
** =====================================================================
*/

void Command::
send_topic(IRC& irc, int fd, const string& channel_name, const string& topic)
{
	if (topic.empty())
		irc.push_cmd_queue(fd, irc.response_to_client(RPL_NOTOPIC, fd, channel_name, RPL_NOTOPIC_MESS));
	else
		irc.push_cmd_queue(fd, irc.response_to_client(RPL_TOPIC, fd, channel_name, topic));
}