#include "Class.Command.hpp"
#include "Class.IRC.hpp"
#include "utils.hpp"

/*
** ====================================================================
** Команда: INVITE
** ====================================================================
** Параметры: <nickname> <channel>
** ====================================================================
** Сообщение INVITE используется для приглашения пользователей на канал. 
** Параметр <nickname> - указание пользователя, которого требуется 
** пригласить на канал, который указывается следующим параметром <channel>. 
** Не обязательно, чтобы канал, на который приглашается указанный 
** пользователь, был отсутствующим или не правильным каналом. Приглашая 
** пользователя на канал, который является invite-only (MODE +i), клиент, 
** посылающий приглашение, должет быть указан как оператор канала на 
** данном канале.
** =====================================================================
*/

void Command::
cmd_invite(IRC& irc, int fd)
{
	User* user = (!_prefix.empty()) ? irc.get_user(_prefix) : irc.get_user(fd);
	if (!user) return;
	
	if (_arguments.size() != 2)
	{
		irc.push_cmd_queue(fd, irc.response(ERR_NEEDMOREPARAMS, user->getName(), _command, ERR_NEEDMOREPARAMS_MESS));
		return;
	}

	User* target = irc.get_user(_arguments[0]);
	if (!target)
	{
		irc.push_cmd_queue(fd, irc.response(ERR_NOSUCHNICK, user->getName(), _arguments[0], ERR_NOSUCHNICK_MESS));
		return;
	}

	irc.push_cmd_queue(fd, irc.fullname(target) + " " +
							utils::int_to_str(RPL_INVITING) + " " +
							user->getName() + " " +
							target->getName() + " " + 
							_arguments[1] + "\r\n");

	irc.push_cmd_queue(target->getSocketFd(),
								irc.fullname(user) +
								" INVITE " +
								target->getName() + " " +
								_arguments[1] + "\r\n");
	
	if (irc.get_channels().count(_arguments[1]))
	{
		ModeChannel* mode = irc.get_channels()[_arguments[1]].getModeChannel();
		mode->invite_masks.push_back(target->getName());
	}
}
