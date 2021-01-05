#include "main.hpp"
#include "Class.Command.hpp"

/*
** ====================================================================
** Команда: JOIN <channel>{,<channel>} [<key>{,<key>}] 
** ====================================================================
** Команда JOIN используется клиентом для входа на канал. Так или иначе, 
** клиенту позволительно войти на канал, проверенным только сервером, к 
** которому подсоединен; все остальные серверы автоматически добавляют 
** пользователя на канал, когда получают уведомление от других серверов. 
** Условия выполнения команды: 
**		1.  Пользователь может быть приглашен, если канал invite-only; 
**		2.  Никнейм/имя пользователя/имя хоста не должны быть 
**  		забанеными; 
**		3.  Пароль (если установлен) должен быть верным. 
** ====================================================================
*/

int Command::cmd_join(IRC& irc, int fd)
{
	if (arguments.empty())
	{
		irc.push_cmd_queue(fd, irc.response_to_client(ERR_NEEDMOREPARAMS, fd, command, ERR_NEEDMOREPARAMS_MESS));
		return 1;
	}

	vector<string> channels = utils::split(arguments[0], ',');
	vector<string> keys = (arguments.size() > 1) ? utils::split(arguments[1], ',') : vector<string>();
	
	for (size_t i = 0; i < channels.size(); i++)
	{
		if (!(channels[i][0] == '&' || channels[i][0] == '#'))
		{
			irc.push_cmd_queue(fd, irc.response_to_client(ERR_NOSUCHCHANNEL, fd, channels[i], ERR_NOSUCHCHANNEL_MESS));
			continue;
		}
		
		string key = (i < keys.size()) ? keys[i] : string();
		string nickname = (prefix.size()) ? prefix : irc.get_user(fd)->getNickname();

		irc.join_channel(channels[i].substr(1), key, channels[i][0], nickname, fd); // channels[i].substr(1) - channel name, channels[i][0] - & or #, prefix - user nickname
	}
	return 0;
}