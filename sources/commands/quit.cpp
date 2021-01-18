#include "Class.Command.hpp"
#include "Class.IRC.hpp"
#include "utils.hpp"

/*
** ====================================================================
** Команда: QUIT
** ====================================================================
** Параметры: [<Quit message>]
** ====================================================================
** Сессия клиента заканчивается с QUIT-сообщением. Сервер должен закрыть 
** соединение с клиентом, когда увидит посланное сообщение. 
** При нетсплите (разрыве соединения между двумя серверами), сообщение 
** QUIT содержит в себе имена двух серверов, разделенные пробелами. 
** Первое имя это сервер, который соединяется, второе имя сервера, который 
** отсоединился. 
** Если, по какой-либо причине, соединение клиента закрылось без введения 
** клиентом команды QUIT (например, обрыв связи), сервер потребует запаса 
** в quit-сообщениях с некоторой сортировкой сообщения, в поисках причины 
** разрыва. 
** =====================================================================
*/

int Command::cmd_quit(IRC& irc, int fd)
{
	User* user = (!prefix.empty()) ? irc.get_user(prefix) : irc.get_user(fd);

	if (user) // если от клиента
	{
		channel_map& map_channels = irc.get_channels();

		//удаляем юзера со всех каналов текущего сервера
		channel_map::iterator it_channels = map_channels.begin();

		while (it_channels != map_channels.end())
		{
			user_map& chann_users = it_channels->second.get_users();	// список пользователей канала

			if (chann_users.count(user))
			{
				chann_users.erase(user);
				if (chann_users.size() == 0) // если канал пуст удалить его из мапы
				{
					map_channels.erase(it_channels);
					it_channels = map_channels.begin();
					continue;
				}
				else //иначе отправить всем пользователем канал сообщение о выходе юзера
				{
					string quit_mess = irc.full_name(user) +
										" QUIT :" +
										(arguments.size() == 1 ? arguments[0] : "Client closed connection");

					irc.forward_to_channel(fd, it_channels->first, quit_mess);
				}
			}
			// удаляем из бан списка
			vector<User*>::iterator it_ban_start = it_channels->second.getVecBanned().begin();
			vector<User*>::iterator it_ban_end = it_channels->second.getVecBanned().end();

			while (it_ban_start != it_ban_end)
			{
				if ((*it_ban_start)->getNickname() == user->getNickname())
				{
					it_channels->second.getVecBanned().erase(it_ban_start);
					break;
				}
				++it_ban_start;
			}
			++it_channels;
		}

		string quit_mess = ":" + user->getName() +
							" QUIT :" +
							(arguments.size() == 1 ? arguments[0] : "Client closed connection");
		
		// отправляем всем серверам информацию о том что клиент вышел
		irc.forward_to_servers(fd, quit_mess, true);
		
		//закрываем соединение, если сообщение от клиента
		if (!is_server(irc, fd))
			irc.close_connection(user);
	}
	return (0);
}
