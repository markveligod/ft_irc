#include "Class.Command.hpp"
#include "Class.IRC.hpp"
#include "utils.hpp"

/*
** ====================================================================
** Команда: WHOIS
** ====================================================================
** Параметры: [<server>] <nickmask>[,<nickmask>[,...]] 
** ====================================================================
** Это сообщение используется для запроса информации об отдельном 
** пользователе. Сервер будет отвечать на это сообщением различными 
** числовыми сообщениями, указывая разность положений каждого пользователя, 
** который попал под маску (если вы указали ее). Если в <nickmask> не 
** указана никакая информация о том, какой никнейм опросить, вы получите 
** информацию о всех доступных никнеймах. Запятая разделяет список 
** никнеймов.
** =====================================================================
*/

void Command::
cmd_whois(IRC& irc, int fd)
{
	User* recepient = (!_prefix.empty()) ? irc.get_user(_prefix) : irc.get_user(fd);
	if (!recepient) return;

	if (_arguments.empty())
	{
		irc.push_cmd_queue(fd, irc.response(ERR_NONICKNAMEGIVEN, recepient->getName(), string(), ERR_NONICKNAMEGIVEN_MESS));
		return;
	}

	if (_arguments.size() == 2)
	{
		if (!(_arguments[0] == irc.get_server_name() ||
				irc.get_server(_arguments[0]) ||
				irc.get_user(_arguments[0])))
		{
			irc.push_cmd_queue(fd, irc.response(ERR_NOSUCHSERVER, recepient->getName(), _command, ERR_NOSUCHSERVER_MESS));
			return;
		}
		// если в _arguments[0] указан не наш сервер, пересылаем сообщение серверу, указанному в _arguments[0]
		if (_arguments[0] != irc.get_server_name() && irc.get_server(_arguments[0]))
		{
			irc.push_cmd_queue(irc.get_server(_arguments[0])->getSocketFd(), _message);
			return;
		}
	}

	vector<string> users = utils::split(_arguments.size() == 1 ? _arguments[0] : _arguments[1], ';'); // получили список пользователей, по которым запрос
	for (size_t i = 0; i < users.size(); i++)
	{
		User* target = irc.get_user(users[i]);
		if (!target)
		{
			irc.push_cmd_queue(fd, irc.response(ERR_NOSUCHNICK, recepient->getName(), users[i], ERR_NOSUCHNICK_MESS));
			continue;
		}
		send_whois(irc, fd, recepient, target);
	}
}

static string
whois_prefix(IRC& irc, int code, User* recepient, User* target)
{
	return string(":" + irc.get_server_name() + " " +
						utils::int_to_str(code) + " " +
						recepient->getName() + " " +
						target->getName() + " ");
}

void Command::
send_whois(IRC& irc, int fd, User* recepient, User* target)
{
	string whois_mess(512, 0);

	{
		whois_mess = whois_prefix(irc, RPL_WHOISUSER, recepient, target) +
					target->getUsername() + " " +
					target->getHostname() + " * :" +
					target->getRealname() + "\r\n";
		irc.push_cmd_queue(fd, whois_mess);
	}
	{
		Server* server = irc.get_server(target->getServername());
		string server_name, server_info;
		
		if (target->getHopcount() == 0)
		{
			server_name = irc.get_server_name();
			server_info = INFO;
		}
		else if (server)
		{
			server_name = server->getName();	
			server_info = server->getInfo();
		}
		else
		{
			server_name = "Unknown server";	
			server_info = " :";
		}
		whois_mess = whois_prefix(irc, RPL_WHOISSERVER, recepient, target) +
						server_name + " :" +
						server_info + "\r\n";
		irc.push_cmd_queue(fd, whois_mess);
	}
	{
		whois_mess = whois_prefix(irc, RPL_WHOISCHANNELS, recepient, target);
		bool first = true;
		for (channel_iterator it = irc.get_channels().begin(); it != irc.get_channels().end(); it++)
		{
			if (it->second.get_users().count(target))		// target присутствует в канале
			{
				whois_mess += (first ? ":" : " ");
				first = false;
				if (it->second.get_users()[target].o)
					whois_mess += "@";
				whois_mess += it->first;
			}
		}
		whois_mess += "\r\n";
		if (!first)
			irc.push_cmd_queue(fd, whois_mess);
	}
	{
		if (target->getMode('o'))
		{
			whois_mess = whois_prefix(irc, RPL_WHOISOPERATOR, recepient, target) +
						RPL_WHOISOPERATOR_MESS + "\r\n";
			irc.push_cmd_queue(fd, whois_mess);
		}
	}
	{
		if (target->getMode('a'))
		{
			whois_mess = whois_prefix(irc, RPL_AWAY, recepient, target) + ":" +
						target->getAway() + "\r\n";
			irc.push_cmd_queue(fd, whois_mess);
		}
	}
	{
		whois_mess = whois_prefix(irc, RPL_ENDOFWHOIS, recepient, target) + RPL_ENDOFWHOIS_MESS + "\r\n";
		irc.push_cmd_queue(fd, whois_mess);
	}
}
