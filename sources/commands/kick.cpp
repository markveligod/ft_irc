#include "Class.Command.hpp"
#include "Class.IRC.hpp"
#include "utils.hpp"

/*
** ====================================================================
** Команда: KICK
** ====================================================================
** Параметры: Параметры: <channel> <user> [<comment>] 
** ====================================================================
** Команда KICK может быть использована для ускоренного удаления 
** пользователя с канала. Как бы выпинывает его с канала (быстрый PART). 
** Только оператора канала может "кикнуть" другого пользователя с канала. 
** Каждый сервер, который получил сообщение KICK, проверяет ее на 
** достоверность (напрмер, что отправитель являтся оператором канала), 
** перед удалением жертвы с канала. 
** =====================================================================
*/

void Command::
cmd_kick(IRC& irc, int fd)
{
	if (_arguments.size() < 2)												// не достаточно параметров
	{
		irc.push_cmd_queue(fd, irc.response(ERR_NEEDMOREPARAMS, fd, _command, ERR_NEEDMOREPARAMS_MESS));
		return;
	}

	User* oper = _prefix.size() ? irc.get_user( _prefix) : irc.get_user(fd);	// сообщение от нусществующего пользователя
	if (!oper)
	{
		irc.push_cmd_queue(fd, irc.response(ERR_NOSUCHNICK, fd, _command, ERR_NOSUCHNICK_MESS));
		return;
	}

	vector<string> channels = utils::split(_arguments[0], ',');
	vector<string> users = utils::split(_arguments[1], ',');

	if (channels.size() > users.size())										// неверный ввод команды
	{
		irc.push_cmd_queue(fd, irc.response(ERR_NEEDMOREPARAMS, fd, _command, ERR_NEEDMOREPARAMS_MESS));
		return;
	}

	else if (channels.size() == users.size())									// удялаем i-го юзера из i-го канала
	{
		for (size_t i = 0; i < channels.size(); i++)
		{
			if (!irc.get_channel(channels[i]))								// канала не существует
			{
				irc.push_cmd_queue(fd, irc.response(ERR_NOSUCHCHANNEL, fd, channels[i], ERR_NOSUCHCHANNEL_MESS));
				continue;
			}
			
			Channel* chann = irc.get_channel(channels[i]);
			User* usr = irc.get_user(users[i]);
			
			if (!chann->is_user_in_channel(oper))							// оператора нет в канале
			{
				irc.push_cmd_queue(fd, irc.response(ERR_NOTONCHANNEL, fd, channels[i], ERR_NOTONCHANNEL_MESS));
				continue;
			}

			if (!chann->is_channel_operator(oper))							// oператор не оператор
			{
				irc.push_cmd_queue(fd, irc.response(ERR_CHANOPRIVSNEEDED, fd, _command, ERR_CHANOPRIVSNEEDED_MESS));
				continue;
			}

			if (!usr || !chann->is_user_in_channel(usr))					// удаляемого пользователя нет в канале
			{
				irc.push_cmd_queue(fd, irc.response(ERR_USERNOTINCHANNEL, fd, channels[i], ERR_USERNOTINCHANNEL_MESS));
				continue;
			}

			string exit_message = (_arguments.size() == 3) ? _arguments[2] : oper->getName();
			string mess_to_user = 		irc.fullname(oper)   + " KICK " + channels[i] + " " + usr->getName();
			string mess_to_server =		":" + oper->getName() + " KICK " + channels[i] + " " + usr->getName();
			irc.forward_to_channel(fd, channels[i], mess_to_user);
			irc.forward_to_servers(fd, mess_to_server);
			if (!irc.is_server(fd))
				irc.push_cmd_queue(fd, mess_to_user + "\r\n");

			chann->get_users().erase(usr);

			if (chann->get_users().empty())
				irc.get_channels().erase(channels[i]);
		}
	}

	else if (channels.size() != 1)												// неверный ввод команды
	{
		irc.push_cmd_queue(fd, irc.response(ERR_NEEDMOREPARAMS, fd, _command, ERR_NEEDMOREPARAMS_MESS));
		return;
	}

	else																	// удаляем список юзеров из одного канала
	{
		string chann_name = channels[0];

		if (!irc.get_channel(chann_name))							// канала не существует
		{
			irc.push_cmd_queue(fd, irc.response(ERR_NOSUCHCHANNEL, fd, chann_name, ERR_NOSUCHCHANNEL_MESS));
			return;
		}
			
		Channel* chann = irc.get_channel(chann_name);

		if (!chann->is_user_in_channel(oper))								// оператора нет в канале
		{
			irc.push_cmd_queue(fd, irc.response(ERR_NOTONCHANNEL, fd, chann_name, ERR_NOTONCHANNEL_MESS));
			return;
		}

		if (!chann->is_channel_operator(oper))								// oператор не оператор
		{
			irc.push_cmd_queue(fd, irc.response(ERR_CHANOPRIVSNEEDED, fd, _command, ERR_CHANOPRIVSNEEDED_MESS));
			return;
		}

		for (size_t i = 0; i < users.size(); i++)
		{
			User* usr = irc.get_user(users[i]);
			
			if (!usr || !chann->is_user_in_channel(usr))						// удаляемого пользователя нет в канале
			{
				irc.push_cmd_queue(fd, irc.response(ERR_USERNOTINCHANNEL, fd, chann_name, ERR_USERNOTINCHANNEL_MESS));
				continue;
			}

			string exit_message = (_arguments.size() == 3) ? _arguments[2] : oper->getName();
			string mess_to_user = 		irc.fullname(oper)   + " KICK " + chann_name + " " + usr->getName();
			string mess_to_server =		":" + oper->getName() + " KICK " + chann_name + " " + usr->getName();
			irc.forward_to_channel(fd, chann_name, mess_to_user);
			irc.forward_to_servers(fd, mess_to_server);

			if (!irc.is_server(fd))
				irc.push_cmd_queue(fd, mess_to_user + "\r\n");

			chann->get_users().erase(usr);

			if (chann->get_users().empty())
				irc.get_channels().erase(chann_name);
		}
	}
}
