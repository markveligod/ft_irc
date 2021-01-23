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

void Command::
cmd_oper(IRC& irc, int fd)
{
	User* usr = irc.get_user(fd);

	if (!usr || irc.is_server(fd))
		return;

	string usr_name = usr->getName();
	if (!check_args_number(2))
	{
		irc.push_cmd_queue(fd, irc.response(ERR_NEEDMOREPARAMS, usr_name, _command, ERR_NEEDMOREPARAMS_MESS));
		return;
	}

	/*	юзер для оператора добавлен в виде prefix*
	 	чтобы можно было добавлять несколько операторов
		ищем длину префикса n, потом сравниваем первые n
		символов ника, запрашиващего операторские права, и префикса
	*/

	if (_arguments[1] == irc.get_operator_pass())
	{
		size_t n = irc.get_operator_user().size() - 1;

		string operator_mask = irc.get_operator_user();
		string user_name = usr_name.substr(0, n) + "*";

		if (_arguments[0] == operator_mask && user_name == operator_mask)
		{
			string mode_mess = " MODE " + usr_name + " :+o\r\n";

			usr->setMode('o', true);
			irc.push_cmd_queue(fd, ":" + irc.get_server_name() + mode_mess);
			irc.push_cmd_queue(fd, irc.response(RPL_YOUREOPER, usr_name, _command, RPL_YOUREOPER_MESS));

			irc.forward_to_servers(fd, ":" + usr_name + mode_mess);
		}
		else
			irc.push_cmd_queue(fd, irc.response(ERR_NOOPERHOST, usr_name, _command, ERR_NOOPERHOST_MESS));
	}
	else
		return irc.push_cmd_queue(fd, irc.response(ERR_PASSWDMISMATCH, usr_name, _command, ERR_PASSWDMISMATCH_MESS));
}