#include "main.hpp"
#include "Class.Command.hpp"
#include "Class.Channel.hpp"
#include "Class.User.hpp"

/*
** =========================================================================
** Команда: MODE 
** Parameters: <nickname> *((+/-) *(i/w/o/O/r))
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
** Parameters: <channel> *((-/+) *<modes> *<modeparams>)
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

void changeMode(User* user, const char param, bool res)
{
	switch (param)
	{
		case 'a': user->getModeUser().a = res;
			break;
		case 'i': user->getModeUser().i = res;
			break;
		case 'w': user->getModeUser().w = res;
			break;
		case 'r': user->getModeUser().r = res;
			break;
		case 'o': user->getModeUser().o = res;
			break;
		case '0': user->getModeUser().O = res;
			break;
		default:
			break;
	}
}

bool check_mode_users(std::string param)
{
	if (param.size() != 2)
		return (false);
	if ((param[0] == '+' || param[0]  == '-') && (param[1] == 'a' || param[1] == 'i' || param[1] == 'w' || param[1] == 'r' || param[1] == 'o' || param[1] == '0'))
		return (true);
	return (false);
}

bool change_mode_channel(Channel *chan, const char param, bool res)
{
	(void)chan;
	(void)param;
	(void)res;
	return (true);
}

int Command::
cmd_mode(IRC& irc, int fd)
{
	std::vector<User*>& vec_users = irc.get_users();
	int pos_oper;

	if (!this->check_args_number(2) && !this->check_args_number(3)) // проверяем на соответствие количеству аргументов
		return (irc.push_mess_client(fd, ERR_NEEDMOREPARAMS));

	if ((pos_oper = irc.find_fd(vec_users, fd)) == -1) // находим предварительную позицию опера
			return (irc.push_mess_client(fd, ERR_USERSDONTMATCH));

	if (vec_users[pos_oper]->getModeUser().o == false && this->command != "OPER") //проверяем опер ли это или нет 
			return (irc.push_mess_client(fd, ERR_USERSDONTMATCH));
	
	if (this->arguments[0][0] == '#') // логика касается модов для каналов
	{
		Channel* curr_channel = irc.get_channel(this->arguments[0]); // получаем канал из списка
		if (curr_channel == NULL)
			return (irc.push_mess_client(fd, ERR_NOSUCHCHANNEL));

		if (this->arguments[1][0] != '+' || this->arguments[1][0] != '-') // проверяем на корректность параметра ключа + и -
			return (irc.push_mess_client(fd, ERR_UNKNOWNMODE));
		
		if (this->arguments.size() == 2) // если указано два параметра то это касается только ключей канала
		{
			for (size_t i = 1; i < this->arguments[1].size(); i++)
			{
				if (!change_mode_channel(curr_channel, this->arguments[1][i], ((this->arguments[1][0] == '+') ? true : false)))
					return (irc.push_mess_client(fd, ERR_UNKNOWNMODE));
			}
		}
		else // иначе мы меняем особые привелегии юзерам на сервере
		{
			
		}
	}
	else // часть кода касается только модов для юзера
	{
		int pos_user;
		
		if (!check_mode_users(this->arguments[1])) // проверяем входные ключи
			return (irc.push_mess_client(fd, ERR_UNKNOWNMODE));
		if ((pos_user = irc.find_name(vec_users, this->arguments[0])) == -1) // находим позицию юзера которого указал опер
			return (irc.push_mess_client(fd, ERR_USERSDONTMATCH));
		changeMode(vec_users[pos_user], this->arguments[1][1], ((this->arguments[1][0] == '+') ? true : false)); // меняем моды
		// ModeUser mode = vec_users[pos_oper]->getModeUser();
		// std::cout << "\nOPER DEBUG:\na: " << mode.a << "\ni: " << mode.i << "\n0: " << mode.O << "\no: " << mode.o << "\nr: " << mode.r << "\nw: " << mode.w << std::endl;
		// mode = vec_users[pos_user]->getModeUser();
		// std::cout << "\nUSER DEBUG:\na: " << mode.a << "\ni: " << mode.i << "\n0: " << mode.O << "\no: " << mode.o << "\nr: " << mode.r << "\nw: " << mode.w << std::endl;
	}
	return (0);
}
