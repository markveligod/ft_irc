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

int Command::cmd_join(IRC& irc, int fd)
{
	vector<string> channels = Utils::split(arguments[0], ',');
	vector<string> keys = (arguments.size() > 1) ? Utils::split(arguments[1], ',') : vector<string>();
	
	for (size_t i = 0; i < channels.size(); i++)
	{
		if (arguments[i][0] != '&' || arguments[i][0] != '#')
			continue;
		string key = (i < keys.size()) ? keys[i] : string();
		irc.join_channel(channels[i].substr(1), key, channels[i][0], prefix, fd); // channels[i].substr(1) - channel name, channels[i][0] - & or #, prefix - user nickname
	}
	return 0;
}