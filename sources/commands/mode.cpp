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

bool check_keys_of_users_mod(std::string arg)
{
	// проверяем размер
	if (arg.size() != 2)
		return (false);
	
	//проверяем ключи
	if ((arg[0] == '+' || arg[0]  == '-') && (arg[1] == 'a' || arg[1] == 'i' || arg[1] == 'w' || arg[1] == 'r' || arg[1] == 'o' || arg[1] == 'O'))
		return (true);
	return (false);
}

bool check_keys_of_channel_mod(std::string arg)
{
	
	char temp[10] = {'a', 'i', 'm', 'n', 'q', 'p', 's', 'r', 't', '\0'};
	// проверяем знак
	if (arg[0] != '+' && arg[0]  != '-')
		return (false);
	
	//проверяем ключи
	for (size_t i = 1; i < arg.size(); i++)
	{
		for (size_t j = 0; j < 9; j++)
		{
			if (arg[i] == temp[j])
				break;
			if ((j + 1 == 9) && (arg[i] != temp[j]))
				return (false);
		}
	}
	return (true);
}

bool check_keys_of_channel_mod_2(std::string arg)
{
	char temp[9] = {'O', 'o', 'v', 'k', 'l', 'b', 'e', 'I', '\0'};
	
	//проверяем размер
	if (arg.size() != 2)
		return (false);
	// проверяем знак
	if (arg[0] != '+' && arg[0]  != '-')
		return (false);
	
	//проверяем ключ
	for (size_t i = 0; i < 8; i++)
	{
		if (arg[1] == temp[i])
			return (true);
	}
	return (false);
}

void change_param_of_users_mod(User *user, const char mod, bool param)
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

void change_param_of_channel_mod(Channel *ch, const char mod, bool param)
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

void push_info(IRC& irc, std::string mess)
{
	std::vector<Server *> vec_servers = irc.get_servers();
	std::vector<User *> vec_users = irc.get_users();
	std::cout << "\nDEBUG: mess: " << mess << std::endl;

	for (size_t i = 0; i < vec_servers.size(); i++)
		irc.forward_to_servers(vec_servers[i]->getSocketFd(), mess, true);
	for (size_t i = 0; i < vec_users.size(); i++)
		irc.push_cmd_queue(vec_users[i]->getSocketFd(), mess + "\r\n");
}

int Command::
cmd_mode(IRC& irc, int fd)
{
	std::cout << "\nDEBUG: prefix: " << this->prefix << std::endl;
	std::cout << "DEBUG: arg:\n";
	for (size_t i = 0; i < this->arguments.size(); i++)
		std::cout << "Index # " << i << " arg: " << this->arguments[i] << std::endl;
	
	// проверяем на соответствие количеству args
	if (!this->check_args_number(2) && !this->check_args_number(3))
		return (irc.push_mess_client(fd, ERR_NEEDMOREPARAMS));

	//Находим клиента который обратился по команде MODE
	User* oper_user = (prefix.size()) ? irc.get_user(prefix) : irc.get_user(fd);
	if (oper_user == NULL)
		return (irc.push_mess_client(fd, ERR_USERSDONTMATCH));

	//проверяем на наличие операторских прав
	if (oper_user->getMode('o') == false)
		return (irc.push_mess_client(fd, 300));

	//проверяем в каком режими нам работать
	if (this->arguments[0][0] == '#' || this->arguments[0][0] == '&')
	{
		//режим канала
		
		//проверяем какие флаги нам подали
		if (check_keys_of_channel_mod(this->arguments[1]) && this->arguments.size() == 2)
		{
			//флаги для переключения

			//ищем канал
			Channel* ch = irc.get_channel(this->arguments[0]);
			if (ch == NULL)
				return (irc.push_mess_client(fd, ERR_NOSUCHCHANNEL));
			
			//меняем моды для итераторной позиции канала
			for (size_t i = 1; i < this->arguments[1].size(); i++)
			{
				change_param_of_channel_mod(ch, this->arguments[1][i], ((this->arguments[1][0] == '+') ? true : false));
			}

			//уведомляем клиентов и сервера об изменениях
			if (prefix.size() == 0)
				push_info(irc, ":" + oper_user->getNickname() + " MODE " + this->arguments[0] + " " + this->arguments[1]);
			
			std::cout << "\nDEBUG: param: " << this->arguments[1] << " DONE!\n";
		}
		else if (check_keys_of_channel_mod_2(this->arguments[1]))
		{
			//флаги для взаимодействия с пользователями

			if ((this->arguments[1][1] == 'O' || this->arguments[1][1] == 'o' || this->arguments[1][1] == 'v') && this->arguments.size() == 3)
			{
				//ищем пользователя которому необходимо присвоить статус создателя канала
				User* _user = irc.get_user(this->arguments[2]);
				if (_user == NULL)
					return (irc.push_mess_client(fd, ERR_USERSDONTMATCH));
				
				//ищем канал
				Channel* ch = irc.get_channel(this->arguments[0]);
				if (ch == NULL)
					return (irc.push_mess_client(fd, ERR_NOSUCHCHANNEL));

				//проверяем пользователь на канале 
				if (!ch->is_user_in_channel(_user))
					return (irc.push_mess_client(fd, ERR_NOTONCHANNEL));
				
				if (this->arguments[1][1] == 'O') // создатель
				{
					if (this->arguments[1][0] == '+')
						ch->set_creator(_user);
					else
						ch->del_operator(_user);
				}
				
				if (this->arguments[1][1] == 'o') // оператор
				{
					if (this->arguments[1][0] == '+')
						ch->set_operator(_user);
					else
						ch->del_operator(_user);
				}

				if (this->arguments[1][1] == 'v') //право голосовать 
				{
					if (this->arguments[1][0] == '+')
						ch->set_voice(_user);
					else
						ch->del_voice(_user);
				}

				//рассылаем уведомление
				if (prefix.size() == 0)
					push_info(irc, ":" + oper_user->getNickname() + " MODE " + this->arguments[0] + " " + this->arguments[1] + " " + this->arguments[2]);
			}
			if ((this->arguments[1][1] == 'k' || this->arguments[1][1] == 'l' || this->arguments[1][1] == 'b' || this->arguments[1][1] == 'e' || this->arguments[1][1] == 'I') && this->arguments.size() == 3)
			{
				//ищем канал
				Channel* ch = irc.get_channel(this->arguments[0]);
				if (ch == NULL)
					return (irc.push_mess_client(fd, ERR_NOSUCHCHANNEL));
				
				if (this->arguments[1][1] == 'k') // ключ на канал
				{
					if (this->arguments[1][0] == '+')
						ch->set_key(this->arguments[2]);
					else
						ch->set_key("");
				}

				if (this->arguments[1][1] == 'l') // ограничение по количеству пользователей
				{
					if (this->arguments[1][0] == '+')
						ch->set_limit_users(atoi(this->arguments[2].c_str()));
					else
						ch->set_limit_users(0);
				}
				
				if (this->arguments[1][1] == 'b') //ban masks
				{
					if (this->arguments[1][0] == '+')
						ch->getModeChannel()->ban_masks.push_back(this->arguments[2]);
					else
					{
						ModeChannel *temp = ch->getModeChannel();
						vector<string>::iterator it_start = temp->ban_masks.begin();
						vector<string>::iterator it_end = temp->ban_masks.end();

						while (it_start != it_end)
						{
							if (*it_start == this->arguments[2])
								break;
							++it_start;
						}
						temp->ban_masks.erase(it_start);
					}
				}

				if (this->arguments[1][1] == 'e') //exception masks
				{
					if (this->arguments[1][0] == '+')
						ch->getModeChannel()->exception_masks.push_back(this->arguments[2]);
					else
					{
						ModeChannel *temp = ch->getModeChannel();
						vector<string>::iterator it_start = temp->exception_masks.begin();
						vector<string>::iterator it_end = temp->exception_masks.end();

						while (it_start != it_end)
						{
							if (*it_start == this->arguments[2])
								break;
							++it_start;
						}
						temp->exception_masks.erase(it_start);
					}
				}

				if (this->arguments[1][1] == 'I') //invite masks
				{
					if (this->arguments[1][0] == '+')
						ch->getModeChannel()->invite_masks.push_back(this->arguments[2]);
					else
					{
						ModeChannel *temp = ch->getModeChannel();
						vector<string>::iterator it_start = temp->invite_masks.begin();
						vector<string>::iterator it_end = temp->invite_masks.end();

						while (it_start != it_end)
						{
							if (*it_start == this->arguments[2])
								break;
							++it_start;
						}
						temp->invite_masks.erase(it_start);
					}
				}

				//рассылаем уведомление
				if (prefix.size() == 0)
					push_info(irc, ":" + oper_user->getNickname() + " MODE " + this->arguments[0] + " " + this->arguments[1] + " " + this->arguments[2]);
			}

			if ((this->arguments[1][1] == 'b' || this->arguments[1][1] == 'e' || this->arguments[1][1] == 'I') && this->arguments.size() == 2)
			{
				//ищем канал
				Channel* ch = irc.get_channel(this->arguments[0]);
				if (ch == NULL)
					return (irc.push_mess_client(fd, ERR_NOSUCHCHANNEL));

				//либо рассылаем сообщание о масках либо очищаем полностью

				if (this->arguments[1][1] == 'b') //ban masks
				{
					if (this->arguments[1][0] == '+')
					{
						for (size_t i = 0; i < ch->getModeChannel()->ban_masks.size(); i++)
							irc.push_cmd_queue(fd, ch->getModeChannel()->ban_masks[i] + "\r\n");
					}
					else
						ch->getModeChannel()->ban_masks.clear();
				}

				if (this->arguments[1][1] == 'e') //exception masks
				{
					if (this->arguments[1][0] == '+')
					{
						for (size_t i = 0; i < ch->getModeChannel()->exception_masks.size(); i++)
							irc.push_cmd_queue(fd, ch->getModeChannel()->exception_masks[i] + "\r\n");
					}
					else
						ch->getModeChannel()->exception_masks.clear();
				}

				if (this->arguments[1][1] == 'I') //invite masks
				{
					if (this->arguments[1][0] == '+')
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
			return (irc.push_mess_client(fd, ERR_UNKNOWNMODE));
	}
	else
	{
		//режим пользователя
		std::cout << "\nDEBUG: Режим пользователя DONE!\n";
		
		//проверяем поступившие ключи от оператора
		if (!check_keys_of_users_mod(this->arguments[2]))
			return (irc.push_mess_client(fd, ERR_UNKNOWNMODE));

		//пытаемся найти пользователя которому необходимо изменить моды
		User* _user = irc.get_user(this->arguments[1]);
		if (_user == NULL)
			return (irc.push_mess_client(fd, ERR_USERSDONTMATCH));
		
		//меняем юзеру параметры
		change_param_of_users_mod(_user, this->arguments[2][1], ((this->arguments[2][0] == '+') ? true : false));

		// если нет префикса шлем уведомление клиентам и серверам об изменение мода
		if (prefix.size() == 0)
			push_info(irc, ":" + oper_user->getNickname() + " MODE " + this->arguments[0] + " " + this->arguments[1] + " " + this->arguments[2]);
		std::cout << "\nDEBUG: param: " << this->arguments[1] << " DONE!\n";
	}
	
	return (0);
}
