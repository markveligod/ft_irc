#include "Class.Command.hpp"
#include "Class.IRC.hpp"
#include "utils.hpp"

/*
** ====================================================================
** Команда: NOTICE
** ====================================================================
** Параметры: <nickname> <text>
** ====================================================================
** Сообщение NOTICE используется подобно PRIVMSG. Отличия между ними в 
** том, что на NOTICE-сообщение ждать автоматического ответа бесполезно. 
** Это правило распространяется и на серверы, - они не должны отсылать 
** обратно сообщения-NOTICE клиентам, содержащие ошибки. Обьект этого 
** правила заключается в петле между клиентом, автоматически посылающим 
** что-либо в ответ на что-либо полученное. Обычно, это используется 
** автоматами (клиентами с AI или другой интерактивной программой, 
** управляющей их действиями). 
** =====================================================================
*/

int Command::cmd_notice(IRC& irc, int fd)
{
	User* sender = (prefix.size()) ? irc.get_user(prefix) : irc.get_user(fd);

	if (!sender || arguments.size() != 2)
		return 1;

	vector<string> recepients = utils::split(arguments[0], ',');

	for (size_t i = 0; i < recepients.size(); i++)
	{
		string message1 = ":" + sender->getName() + " " + "NOTICE" + " " +
							 recepients[i] + " " + ":" + arguments[1] + "\r\n";
		string message2 = ":" + irc.full_name(sender) + " " + "NOTICE" + " " +
							 recepients[i] + " " + ":" + arguments[1] + "\r\n";

		Channel* channel = irc.get_channel(recepients[i]);
		if (channel)
		{
			bool in_channel = !channel->is_user_in_channel(sender);
			ModeUser mode_user = in_channel ? channel->get_users()[sender] : ModeUser();

			if (!((channel->getModeChannel('n') && !in_channel)				// пользователь не в канале и запрет в канале прием сообщений извне
				|| channel->is_banned(sender)								// отправитель забанен в канале
				|| (channel->getModeChannel('m')
					&& (!in_channel || !(mode_user.o || mode_user.v)))))	// модерируемый канал и пользователь не в канале или не имеет право говорить
			{

				irc.forward_to_servers(fd, message1, true);
				irc.forward_to_channel(fd, recepients[i], message2);
			}
			continue;
		}

		User* user = irc.get_user(recepients[i]);
		if (user)
			irc.push_cmd_queue(user->getSocketFd(), message2);
	}

	return 0;
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
