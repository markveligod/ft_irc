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

	if (_arguments.size() != 2)												// слишком много аргуменотов
	{
		irc.push_cmd_queue(fd, irc.response(ERR_NEEDMOREPARAMS, fd, _command, ERR_NEEDMOREPARAMS_MESS));
		return;
	}

	string response = ":" + user->getName() + " PRIVMSG :";
	User* pogoda = irc.get_user("pogoda");
	
	if (!pogoda)
		return (irc.push_cmd_queue(fd, response + "bot \"POGODA\" not available\r\n"));
	
	return (irc.push_cmd_queue(fd, response + _arguments[1] + "\r\n"));
}
