#include "Class.Command.hpp"
#include "Class.IRC.hpp"
#include "utils.hpp"

/*
** ====================================================================
** Команда: PRIVMSG
** ====================================================================
** Параметры: <receiver>{,<receiver>} <text to be sent>
** ====================================================================
** PRIVMSG используется для частной переписки между пользователями. 
** <receiver> - никнейм получателя сообщения. Так же там можно указать 
** список имен или каналов, разделенных запятыми.
** =====================================================================
*/

void Command::
cmd_privmsg(IRC& irc, int fd)
{
	User* sender = (_prefix.size()) ? irc.get_user(_prefix) : irc.get_user(fd);

	if (!sender)
	{
		irc.push_cmd_queue(fd, irc.response(ERR_NOSUCHNICK, fd, _prefix, ERR_NOSUCHNICK_MESS));
		return;
	}

	if (_arguments.empty())
	{
		irc.push_cmd_queue(fd, irc.response(ERR_NORECIPIENT, fd, _prefix, string(ERR_NORECIPIENT_MESS) + " (PRIVMSG)"));
		return;
	}

	if (_arguments.size() == 1)
	{
		irc.push_cmd_queue(fd, irc.response(ERR_NOTEXTTOSEND, fd, _prefix, ERR_NOTEXTTOSEND_MESS));
		return;
	}

	if (_arguments.size() > 2)
	{
		irc.push_cmd_queue(fd, irc.response(ERR_NEEDMOREPARAMS, fd, _prefix, ERR_NEEDMOREPARAMS_MESS));
		return;
	}

	vector<string> recepients = utils::split(_arguments[0], ',');

	for (size_t i = 0; i < recepients.size(); i++)
	{
		string message = ":" + sender->getName() + " " +
							   "PRIVMSG" + " " +
							   recepients[i] + " " +
						 ":" + _arguments[1];

		Channel* channel = irc.get_channel(recepients[i]);
		if (channel)
		{
			bool in_channel = channel->is_user_in_channel(sender);
			ModeUser mode_user = in_channel ? channel->get_users()[sender] : ModeUser();

			if ((channel->getModeChannel('n') && !in_channel)			// пользователь не в канале и запрет в канале прием сообщений извне
				|| channel->is_banned(sender)							// отправитель забанен в канале
				|| (channel->getModeChannel('m')
					&& (!in_channel || !(mode_user.o || mode_user.v))))	// модерируемый канал и пользователь не в канале или не имеет право говорить
			{
				irc.push_cmd_queue(fd, irc.response(ERR_CANNOTSENDTOCHAN, fd, _prefix,
													recepients[i] + " " + string(ERR_CANNOTSENDTOCHAN_MESS)));
			}
			else
			{
				irc.forward_to_channel(fd, recepients[i], message);
				irc.forward_to_servers(fd, message);
			}
			continue;
		}

		User* user = irc.get_user(recepients[i]);
		if (user)
		{
			if (user->getMode('a'))
				irc.push_cmd_queue(fd, irc.response(RPL_AWAY, fd, recepients[i], ":" + user->getAway()));
			else
				irc.push_cmd_queue(user->getSocketFd(), message + "\r\n");
			continue;
		}
		irc.push_cmd_queue(fd, irc.response(ERR_NOSUCHNICK, fd, _prefix, ERR_NOSUCHNICK_MESS));
	}
}

// личное сообщение от клиента для cddoma
// PRIVMSG cddoma :hello
// :poli PRIVMSG cddoma :hello

// сообщение в канал
// PRIVMSG #q :hello
// :poli PRIVMSG #q :hello

// одновременно и туда, и туда
// PRIVMSG #q,poli :hello
// :poli PRIVMSG #q :hello
// :poli PRIVMSG poli :hello
