#include "Class.Command.hpp"
#include "Class.IRC.hpp"
#include "utils.hpp"

/*
** ====================================================================
** Команда: POGODA
** ====================================================================
** Параметры: Город
** ====================================================================
** Получает сообщение от клиента и если подключен бот pogoda, 
** то формирует PRIVMSG названием города и отправляет его боту, иначе
** сообщает, что бот не подключен.
** =====================================================================
*/

void Command::
cmd_pogoda(IRC& irc, int fd)
{
	User* user = (!_prefix.empty()) ? irc.get_user(_prefix) : irc.get_user(fd);
	if (!user) return;

	if (!(_arguments.size() == 2 || (_prefix.empty() && _arguments.size() == 1)))												// слишком много аргуменотов
	{
		irc.push_cmd_queue(fd, irc.response(ERR_NEEDMOREPARAMS, fd, _command, ERR_NEEDMOREPARAMS_MESS));
		return;
	}

	string response = ":" + user->getName() + " PRIVMSG pogoda :";
	User* pogoda = irc.get_user("pogoda");
	
	if (!pogoda)
		return (irc.push_cmd_queue(fd, "Bot \"POGODA\" not available\r\n"));
	
	string city = _prefix.empty() ? _arguments[0] : _arguments[1];
	return (irc.push_cmd_queue(pogoda->getSocketFd(), response + city + "\r\n"));
}
