#include "Class.Command.hpp"
#include "Class.IRC.hpp"
#include "utils.hpp"

/*
** ====================================================================
** Команда: OPER 
** ====================================================================
** Параметры: <user> <password> 
** ====================================================================
** Сообщение OPER используется нормальным пользователем для взятия 
** операторских привилегий. Комбинация <user> и <password> используется 
** для идентификации пользователя, запрашивающего права IRC-оператора. 
** Если клиент послал команду OPER с корректным паролем для текущего 
** пользователя, сервер информамирует сеть о новом операторе, используя 
** "MODE +o" для никнейма клиента. 
** Сообщение OPER только для клиент-сервер.
** Пример: 
** OPER foo bar 
**
** Попытка зарегистрироваться как оператору, используя имя пользователя "foo" и пароль "bar".
** =====================================================================
*/

int Command::
cmd_oper(IRC& irc, int fd)
{
	if (!this->check_args_number(2))
		return (irc.push_mess_client(fd, ERR_NEEDMOREPARAMS));

	User* usr = irc.get_user(fd);

	if (!usr)
		return (irc.push_mess_client(fd, ERR_NOTREGISTERED));

	/*	юзер для оператора добавлен в виде prefix*
	 	чтобы можно было добавлять несколько операторов
		ищем длину префикса n, потом сравниваем первые n
		символов ника, запрашиващего операторские права, и префикса
	*/

	if (arguments[1] == irc.get_operator_pass())
	{
		size_t n = irc.get_operator_user().size() - 1;

		string operator_mask = irc.get_operator_user();
		string user_name = usr->getName().substr(0, n) + "*";

		if (arguments[0] == operator_mask && user_name == operator_mask)
		{
			string mode_mess = " MODE " + usr->getName() + " :+o\r\n";

			usr->setMode('o', true);
			irc.push_cmd_queue(fd, irc.response(RPL_YOUREOPER, fd, command, RPL_YOUREOPER_MESS));
			irc.push_cmd_queue(fd, irc.get_server_name() + mode_mess);

			irc.forward_to_servers(fd, usr->getName() + mode_mess);
		}
		else
			irc.push_cmd_queue(fd, irc.response(ERR_NOOPERHOST, fd, command, ERR_NOOPERHOST_MESS));
	}
	else
		return (irc.push_mess_client(fd, ERR_PASSWDMISMATCH));
	return (0);
}