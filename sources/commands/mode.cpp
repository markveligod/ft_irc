#include "main.hpp"
#include "Class.Command.hpp"
#include "Class.Channel.hpp"
#include "Class.User.hpp"

/*
** =========================================================================
** Команда: MODE 
** =========================================================================
** Параметры: <nickname> *((+/-) *(i/w/o/O/r))
** =========================================================================
** Пользовательские РЕЖИМЫ обычно представляют собой изменения, которые
** влияют либо на то, как клиента видят другие, либо на то, какие
** «дополнительные» сообщения отправляет клиент.
** Пользовательская команда MODE ДОЛЖНА быть принята только в том случае,
** если и отправитель сообщения, и псевдоним, указанный в качестве
** параметра,одинаковы. Если не указан другой параметр, то сервер вернет
** текущие настройки для ника.
** Доступные режимы:
**	a - пользователь отмечен как отсутствующий;
**	i - отмечает пользователя как невидимого;
**	w - пользователь получает валлопы;
**	r - ограниченное подключение пользователя;
**	o - флаг оператора;
**	O - флаг местного оператора;
**
** Флаг 'a' НЕ ДОЛЖЕН переключаться пользователем с помощью команды MODE,
** вместо этого ТРЕБУЕТСЯ использование команды AWAY.
** Если пользователь пытается сделать себя оператором, используя флаг
** «+ o» или «+ O», эту попытку СЛЕДУЕТ проигнорировать, поскольку
** пользователи могут обойти механизмы аутентификации команды OPER. Однако
** нет никаких ограничений на то, чтобы кто-либо "деопировал" себя
** (используя "-o" или "-O").
** С другой стороны, если пользователь пытается сделать себя неограниченным,
** используя флаг «-r», эту попытку СЛЕДУЕТ игнорировать. Однако нет никаких
** ограничений на то, чтобы кто-либо "деопировал" себя (используя "+r").
** Этот флаг обычно устанавливается сервером при подключении по
** административным причинам. Хотя наложенные ограничения оставлены на
** усмотрение реализации, обычно ограниченному пользователю не разрешается
** изменять псевдонимы или использовать статус оператора канала на каналах.
** =========================================================================
**
**
** =========================================================================
** Команда: MODE 
** =========================================================================
** Параметры: <channel> *((-/+) *<modes> *<modeparams>)
** =========================================================================
** Команда MODE предназначена для того, чтобы пользователи могли запрашивать
** иизменять характеристики канала. Cуществует максимальное ограничение
** в три (3) изменения на команду для режимов, которые принимают параметр.
** Для каналов доступны следующие различные режимы:

**	O - присвоить статус «создатель канала»;
**	o - дать / принять привилегию оператора канала;
**	v - дать / принять право голоса;
**	a - переключить флаг анонимного канала;
**	i - переключить флаг канала только по приглашению;
**	m - переключить модерируемый канал;
**	n - переключить отсутствие сообщений на канал от клиентов снаружи;
**	q - переключить флаг тихого канала;
**	p - переключить флаг приватного канала;
**	s - переключить флаг секретного канала;
**	r - переключить флаг канала перезапуска сервера;
**	t - переключить TOPIC, устанавливаемую только оператором канала;
**	k - установить / удалить ключ канала (пароль);
**	l - установить / снять лимит пользователя на канал;
**	b - установить / удалить маску бана, чтобы пользователи не заходили;
**	e - установить / удалить маску исключения для отмены маски бана;
**	I - установить / удалить маску приглашения для автоматического переопределения
**		флаг "только для приглашения";
*/

bool
check_keys_of_users_mod(string arg)
{
	// проверяем размер
	if (arg.size() != 2)
		return false;
	
	//проверяем ключи
	if ((arg[0] == '+' || arg[0]  == '-') && (arg[1] == 'a' || arg[1] == 'i' || arg[1] == 'w' || arg[1] == 'r' || arg[1] == 'o' || arg[1] == 'O'))
		return true;
	return false;
}

bool
check_keys_of_channel_mod(string arg)
{
	
	char temp[10] = {'a', 'i', 'm', 'n', 'q', 'p', 's', 'r', 't', '\0'};
	// проверяем знак
	if (arg[0] != '+' && arg[0]  != '-')
		return false;
	
	//проверяем ключи
	for (size_t i = 1; i < arg.size(); i++)
	{
		for (size_t j = 0; j < 9; j++)
		{
			if (arg[i] == temp[j])
				break;
			if ((j + 1 == 9) && (arg[i] != temp[j]))
				return false;
		}
	}
	return true;
}

bool
check_keys_of_channel_mod_2(string arg)
{
	char temp[9] = {'O', 'o', 'v', 'k', 'l', 'b', 'e', 'I', '\0'};
	
	//проверяем размер
	if (arg.size() != 2)
		return false;
	// проверяем знак
	if (arg[0] != '+' && arg[0]  != '-')
		return false;
	
	//проверяем ключ
	for (size_t i = 0; i < 8; i++)
	{
		if (arg[1] == temp[i])
			return true;
	}
	return false;
}

void
change_param_of_users_mod(User *user, const char mod, bool param)
{
	switch (mod)
	{
		case 'a': user->getModeUser().a = param;
			break;
		case 'i': user->getModeUser().i = param;
			break;
		case 'w': user->getModeUser().w = param;
			break;
		case 'r': user->getModeUser().r = param;
			break;
		case 'o': user->getModeUser().o = param;
			break;
		case 'O': user->getModeUser().O = param;
			break;
		default:
			break;
	}
}

void
change_param_of_channel_mod(Channel *ch, const char mod, bool param)
{
	/*
	**	a - переключить флаг анонимного канала;
	**	i - переключить флаг канала только по приглашению;
	**	m - переключить модерируемый канал;
	**	n - переключить отсутствие сообщений на канал от клиентов снаружи;
	**	q - переключить флаг тихого канала;
	**	p - переключить флаг приватного канала;
	**	s - переключить флаг секретного канала;
	**	r - переключить флаг канала перезапуска сервера;
	**	t - переключить TOPIC, устанавливаемую только оператором канала;
	*/
	switch (mod)
	{
		case 'a': ch->getModeChannel()->a = param;
			break;
		case 'i': ch->getModeChannel()->i = param;
			break;
		case 'm': ch->getModeChannel()->m = param;
			break;
		case 'n': ch->getModeChannel()->n = param;
			break;
		case 'q': ch->getModeChannel()->q = param;
			break;
		case 'p': ch->getModeChannel()->p = param;
			break;
		case 's': ch->getModeChannel()->s = param;
			break;
		case 'r': ch->getModeChannel()->r = param;
			break;
		case 't': ch->getModeChannel()->t = param;
			break;
		default:
			break;
	}
}

void Command::
cmd_mode(IRC& irc, int fd)
{
	
	std::cout << "\nDEBUG: prefix: " << _prefix << std::endl;
	std::cout << "DEBUG: arg:\n";
	for (size_t i = 0; i < _arguments.size(); i++)
		std::cout << "Index # " << i << " arg: " << _arguments[i] << std::endl;

	//Находим клиента который обратился по команде MODE
	User* oper_user = (_prefix.size()) ? irc.get_user(_prefix) : irc.get_user(fd);
	if (!oper_user) return;
	string oper_name = oper_user->getName();

	//если нам подают один аргумент с названием канала
	if ((_arguments[0][0] == '#' || _arguments[0][0] == '&') && _arguments.size() == 1)
	{
		string chan_name = _arguments[0];
	
		if (_prefix.size() == 0)
		{
			if (!irc.get_channels().count(chan_name))
			{
				irc.push_cmd_queue(fd, irc.response(ERR_NOSUCHCHANNEL, oper_name, chan_name, ERR_NOSUCHCHANNEL_MESS));
				return;
			}

			string mode_mess = " MODE " + chan_name + " +";
		
			ModeChannel* mode = irc.get_channels()[chan_name].getModeChannel();
			if (mode->p) irc.push_cmd_queue(fd, ":" + oper_name + mode_mess + "p" + "\r\n");
			if (mode->s) irc.push_cmd_queue(fd, ":" + oper_name + mode_mess + "s" + "\r\n");
			if (mode->i) irc.push_cmd_queue(fd, ":" + oper_name + mode_mess + "i" + "\r\n");
			if (mode->t) irc.push_cmd_queue(fd, ":" + oper_name + mode_mess + "t" + "\r\n");
			if (mode->m) irc.push_cmd_queue(fd, ":" + oper_name + mode_mess + "m" + "\r\n");
			if (mode->l) irc.push_cmd_queue(fd, ":" + oper_name + mode_mess + "l" + "\r\n");
			if (mode->k) irc.push_cmd_queue(fd, ":" + oper_name + mode_mess + "k" + "\r\n");
			if (mode->a) irc.push_cmd_queue(fd, ":" + oper_name + mode_mess + "a" + "\r\n");
			if (mode->n) irc.push_cmd_queue(fd, ":" + oper_name + mode_mess + "n" + "\r\n");
			if (mode->q) irc.push_cmd_queue(fd, ":" + oper_name + mode_mess + "q" + "\r\n");
		}
	}

	// проверяем на соответствие количеству args
	if (!check_args_number(2) && !check_args_number(3))
	{
		irc.push_cmd_queue(fd, irc.response(ERR_NEEDMOREPARAMS, oper_name, _arguments[0], ERR_NEEDMOREPARAMS_MESS));
		return;
	}

	string chan_name = _arguments[0];
	//проверяем в каком режиме нам работать
	if (_arguments[0][0] == '#' || _arguments[0][0] == '&')
	{
		Channel* ch = irc.get_channel(chan_name);	//ищем канал
		if (ch == NULL)
		{
			irc.push_cmd_queue(fd, irc.response(ERR_NOSUCHCHANNEL, oper_name, chan_name, ERR_NOSUCHCHANNEL_MESS));
			return;
		}

		user_map& users = ch->get_users();
		if (!users.count(oper_user))				//проверяем, что oper_user находится в канале
		{
			irc.push_cmd_queue(fd, irc.response(ERR_NOSUCHNICK, oper_name, chan_name, ERR_NOSUCHNICK_MESS));
			return;			
		}

		//режим канала
													//проверяем какие флаги нам подали
		if (check_keys_of_channel_mod(_arguments[1]) && _arguments.size() == 2)
		{
			//флаги для переключения

			if (!users[oper_user].o)				//проверяем на наличие операторских прав в канале
			{
				irc.push_cmd_queue(fd, irc.response(ERR_CHANOPRIVSNEEDED, oper_name, chan_name, ERR_CHANOPRIVSNEEDED_MESS));
				return;
			}

													//меняем моды для итераторной позиции канала
			for (size_t i = 1; i < _arguments[1].size(); i++)
			{
				change_param_of_channel_mod(ch, _arguments[1][i], ((_arguments[1][0] == '+') ? true : false));
			}

			//уведомляем клиентов и серверы об изменениях
			if (_prefix.size() == 0)
			{
				string mode_mess = " MODE " + chan_name + " " + _arguments[1];
				irc.forward_to_servers(fd, ":" + oper_name + mode_mess);
				irc.forward_to_channel(fd, chan_name, irc.fullname(oper_user) + mode_mess);
			}
			
			std::cout << "\nDEBUG: param: " << _arguments[1] << " DONE!\n";
		}
		else if (check_keys_of_channel_mod_2(_arguments[1]))
		{
			//флаги для взаимодействия с пользователями

			if ((_arguments[1][1] == 'O' || _arguments[1][1] == 'o' || _arguments[1][1] == 'v') && _arguments.size() == 3)
			{
				//ищем пользователя, которому необходимо присвоить статус создателя канала
				User* _user = irc.get_user(_arguments[2]);
				if (_user == NULL)
				{
					irc.push_cmd_queue(fd, irc.response(ERR_USERSDONTMATCH, oper_name, _arguments[2], ERR_NOSUCHNICK_MESS));
					return;
				}

				//проверяем пользователь на канале 
				if (!ch->is_user_in_channel(_user))
				{
					irc.push_cmd_queue(fd, irc.response(ERR_USERNOTINCHANNEL, oper_name, _arguments[2], ERR_USERNOTINCHANNEL_MESS));
					return;
				}
				
				if (_arguments[1][1] == 'O') // создатель
				{
					if (_arguments[1][0] == '+')
						ch->set_creator(_user);
					else
						ch->del_operator(_user);
				}
				
				if (_arguments[1][1] == 'o') // оператор
				{
					if (_arguments[1][0] == '+')
						ch->set_operator(_user);
					else
						ch->del_operator(_user);
				}

				if (_arguments[1][1] == 'v') //право голосовать 
				{
					if (_arguments[1][0] == '+')
						ch->set_voice(_user);
					else
						ch->del_voice(_user);
				}

				//рассылаем уведомление
				string mode_mess = " MODE " + chan_name + " " + _arguments[1] + " " + _arguments[2];
				irc.forward_to_servers(fd, ":" + oper_name + mode_mess);
				irc.forward_to_channel(fd, chan_name, irc.fullname(oper_user) + mode_mess);
			}
			if ((_arguments[1][1] == 'k' || _arguments[1][1] == 'l' || _arguments[1][1] == 'b' || _arguments[1][1] == 'e' || _arguments[1][1] == 'I') && _arguments.size() == 3)
			{
				//ищем канал
				if (_arguments[1][1] == 'k') // ключ на канал
				{
					if (_arguments[1][0] == '+')
						ch->set_key(_arguments[2]);
					else
						ch->set_key("");
					
				}

				if (_arguments[1][1] == 'l') // ограничение по количеству пользователей
				{
					if (_arguments[1][0] == '+')
						ch->set_limit_users(atoi(_arguments[2].c_str()));
					else
						ch->set_limit_users(0);
				}
				
				if (_arguments[1][1] == 'b') //ban masks
				{
					if (_arguments[1][0] == '+')
						ch->getModeChannel()->ban_masks.push_back(_arguments[2]);
					else
					{
						ModeChannel *temp = ch->getModeChannel();
						vector<string>::iterator it_start = temp->ban_masks.begin();
						vector<string>::iterator it_end = temp->ban_masks.end();

						while (it_start != it_end)
						{
							if (*it_start == _arguments[2])
								break;
							++it_start;
						}
						temp->ban_masks.erase(it_start);
					}
				}

				if (_arguments[1][1] == 'e') //exception masks
				{
					if (_arguments[1][0] == '+')
						ch->getModeChannel()->exception_masks.push_back(_arguments[2]);
					else
					{
						ModeChannel *temp = ch->getModeChannel();
						vector<string>::iterator it_start = temp->exception_masks.begin();
						vector<string>::iterator it_end = temp->exception_masks.end();

						while (it_start != it_end)
						{
							if (*it_start == _arguments[2])
								break;
							++it_start;
						}
						temp->exception_masks.erase(it_start);
					}
				}

				if (_arguments[1][1] == 'I') //invite masks
				{
					if (_arguments[1][0] == '+')
						ch->getModeChannel()->invite_masks.push_back(_arguments[2]);
					else
					{
						ModeChannel *temp = ch->getModeChannel();
						vector<string>::iterator it_start = temp->invite_masks.begin();
						vector<string>::iterator it_end = temp->invite_masks.end();

						while (it_start != it_end)
						{
							if (*it_start == _arguments[2])
								break;
							++it_start;
						}
						temp->invite_masks.erase(it_start);
					}
				}

				//рассылаем уведомление
				string mode_mess = " MODE " + chan_name + " " + _arguments[1] + " " + _arguments[2];
				irc.forward_to_servers(fd, ":" + oper_name + mode_mess);
				irc.forward_to_channel(fd, chan_name, irc.fullname(oper_user) + mode_mess);
			}

			if ((_arguments[1][1] == 'b' || _arguments[1][1] == 'e' || _arguments[1][1] == 'I') && _arguments.size() == 2)
			{
				//либо рассылаем сообщание о масках либо очищаем полностью

				if (_arguments[1][1] == 'b') //ban masks
				{
					if (_arguments[1][0] == '+')
					{
						for (size_t i = 0; i < ch->getModeChannel()->ban_masks.size(); i++)
							irc.push_cmd_queue(fd, ch->getModeChannel()->ban_masks[i] + "\r\n");
					}
					else
						ch->getModeChannel()->ban_masks.clear();
				}

				if (_arguments[1][1] == 'e') //exception masks
				{
					if (_arguments[1][0] == '+')
					{
						for (size_t i = 0; i < ch->getModeChannel()->exception_masks.size(); i++)
							irc.push_cmd_queue(fd, ch->getModeChannel()->exception_masks[i] + "\r\n");
					}
					else
						ch->getModeChannel()->exception_masks.clear();
				}

				if (_arguments[1][1] == 'I') //invite masks
				{
					if (_arguments[1][0] == '+')
					{
						for (size_t i = 0; i < ch->getModeChannel()->invite_masks.size(); i++)
							irc.push_cmd_queue(fd, ch->getModeChannel()->invite_masks[i] + "\r\n");
					}
					else
						ch->getModeChannel()->invite_masks.clear();
				}
			}
		}
		else
		{
			irc.push_cmd_queue(fd, irc.response(ERR_UNKNOWNMODE, oper_name, _arguments[1], ERR_UNKNOWNMODE_MESS));
			return;
		}
	}
	else
	{
		if (_arguments.size() < 3)
		{
			irc.push_cmd_queue(fd, irc.response(ERR_NEEDMOREPARAMS, oper_name, _arguments[1], ERR_NEEDMOREPARAMS_MESS));
			return;
		}
		//режим пользователя
		std::cout << "\nDEBUG: Режим пользователя DONE!\n";
		
		//проверяем поступившие ключи от оператора
		if (!check_keys_of_users_mod(_arguments[2]))
		{
			irc.push_cmd_queue(fd, irc.response(ERR_UNKNOWNMODE, oper_name, _arguments[1], ERR_UNKNOWNMODE_MESS));
			return;
		}

		//пытаемся найти пользователя которому необходимо изменить моды
		User* _user = irc.get_user(_arguments[1]);
		if (_user == NULL)
		{
			irc.push_cmd_queue(fd, irc.response(ERR_USERSDONTMATCH, oper_name, _arguments[1], ERR_USERSDONTMATCH_MESS));
			return;
		}
		
		//меняем юзеру параметры
		change_param_of_users_mod(_user, _arguments[2][1], ((_arguments[2][0] == '+') ? true : false));

		// если нет префикса шлем уведомление клиентам и серверам об изменение мода
		if (_prefix.size() == 0)
		{
			string mode_mess = " MODE " + chan_name + " " + _arguments[1] + " " + _arguments[2];
			irc.forward_to_servers(fd, ":" + oper_name + mode_mess);
			irc.forward_to_channel(fd, chan_name, irc.fullname(oper_user) + mode_mess);
		}
		std::cout << "\nDEBUG: param: " << _arguments[1] << " DONE!\n";
	}
}
