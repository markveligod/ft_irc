#include "main.hpp"
#include "Class.Command.hpp"

using std::string;
using std::vector;
using std::map;

/*
** ====================================================================
** Команда: JOIN <channel>{,<channel>} [<key>{,<key>}] 
** ====================================================================
** 
** ====================================================================
*/

int Command::
cmd_join(IRC& irc, int fd)
{
	vector<string> channels = utils::split(arguments[0], ',');
	vector<string> keys = (arguments.size() > 1) ? utils::split(arguments[1], ',') : vector<string>();
	
	for (size_t i = 0; i < channels.size(); i++)
	{
		if (arguments[i].empty())
		{
			irc.push_cmd_queue(fd, utils::convert_int_to_str(ERR_NEEDMOREPARAMS));
			continue;
		}
		if (!(arguments[i][0] == '&' || arguments[i][0] == '#'))
		{
			irc.push_cmd_queue(fd, utils::convert_int_to_str(ERR_NOSUCHCHANNEL));
			continue;
		}
		string key = (i < keys.size()) ? keys[i] : string();
		irc.join_channel(channels[i].substr(1), key, channels[i][0], prefix, fd); // channels[i].substr(1) - channel name, channels[i][0] - & or #, prefix - user nickname
	}
	return 0;
}