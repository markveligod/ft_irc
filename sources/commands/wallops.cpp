#include "Class.Command.hpp"
#include "Class.IRC.hpp"
#include "utils.hpp"

/*
** ====================================================================
** Команда: WALLOPS
** ====================================================================
** Параметры: [message]
** ====================================================================
** Отправка сообщения всем IRC-операторам, которые находятся в данный 
** момент в сети. После выполнения WALLOPS как команды пользователя, 
** это будет расцениваться как частое и обычно неверное, ибо отправлено 
** сообщение большому количеству людей (подобно WALL).
** =====================================================================
*/

int Command::cmd_wallops(IRC& irc, int fd)
{
	if (arguments.size() != 1)
		return (irc.push_mess_client(fd, ERR_NEEDMOREPARAMS));

	vector<User*>& users = irc.get_users();
	string wallop = ":" + irc.get_server_name() + " WALLOPS :" + arguments[0] + "\r\n";

	for (vector<User*>::iterator it = users.begin(); it != users.end(); it++)
	{
		if ((*it)->getHopcount() == 0 && ((*it)->getMode('o') || (*it)->getMode('w')))
			irc.push_cmd_queue(fd, wallop);
	}

	return 0;
}
