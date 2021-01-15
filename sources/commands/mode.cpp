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

void changeMode(ModeUser* mode, const char param, bool res)
{
	switch (param)
	{
		case 'a': mode->a = res;
			break;
		case 'i': mode->i = res;
			break;
		case 'w': mode->w = res;
			break;
		case 'r': mode->r = res;
			break;
		case 'o': mode->o = res;
			break;
		case '0': mode->O = res;
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

int Command::
cmd_mode(IRC& irc, int fd)
{
	if (!this->check_args_number(2) && !this->check_args_number(3))
		return (irc.push_mess_client(fd, ERR_NEEDMOREPARAMS));
	if (this->arguments[0][0] == '#' || this->arguments[0][0] == '&')
	{
		utils::print_line("Этот канал " + this->arguments[0] + " запрашивает права " + this->arguments[1]);
	}
	else
	{
		std::vector<User*>& vec_users = irc.get_users();
		int pos;

		if (!check_mode_users(this->arguments[1]))
			return (irc.push_mess_client(fd, ERR_UNKNOWNMODE));
		if ((pos = irc.find_fd(vec_users, fd)) == -1)
			return (irc.push_mess_client(fd, ERR_USERSDONTMATCH));
		if (vec_users[pos]->getName() != this->arguments[0])
			return (irc.push_mess_client(fd, ERR_NOSUCHNICK));
		ModeUser mode = vec_users[pos]->getModeUser();
		std::cout << "\nBEFORE DEBUG:\na: " << mode.a << "\ni: " << mode.i << "\n0: " << mode.O << "\no: " << mode.o << "\nr: " << mode.r << "\nw: " << mode.w << std::endl;
		//if (mode.o == false && this->command != "OPER")
		//	return (irc.push_mess_client(fd, ERR_USERSDONTMATCH));
		changeMode(&mode, this->arguments[1][1], ((this->arguments[1][0] == '+') ? true : false));
		vec_users[pos]->setMode(mode);
		std::cout << "\nAFTER DEBUG:\na: " << mode.a << "\ni: " << mode.i << "\n0: " << mode.O << "\no: " << mode.o << "\nr: " << mode.r << "\nw: " << mode.w << std::endl;
	}
	return (0);
}
