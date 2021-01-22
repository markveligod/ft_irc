#include "main.hpp"
#include "Class.Command.hpp"

/*
** ================================================================
** Команда: PASS
** ================================================================
** Параметры: <password>
** ================================================================
** cmd_pass		-	при удачном вызове меняет статус 
**					класса Client на корректный ввод пароля
** принимает	-	var_1 - указатель на вектор клиентов
**					var_2 - файловый дескриптор сокета, по которому
**							пришел запрос на выполнение команды
** суть работы	-	если пароль соответствует паролю сервера,
**					ищем по вектору всех подключенных клиентов,
**					(при помощи fd сокета),
**					от кого пришел запрос на вход с таким паролем,
**					и этому клиенту меняем статус пароля на true
** =================================================================
*/

void Command::
cmd_pass(IRC& irc, int fd)
{
	int i;
	bool res;
	vector<Client*>& clients	= irc.get_clients();
	vector<Server*>& servers 	= irc.get_servers();
	
	//if (!(check_args_number(1)))
	//	return (ERR_NEEDMOREPARAMS);

	if ((i = IRC::find_fd(clients, fd)) < 0)
		return;								// по идее, такой ситуации быть не может

	if (clients[i]->getPassword() || IRC::find_fd(servers, fd) >= 0)
	{
		utils::print_error(ERR_ALREADYREGISTRED, "Already registered");
		return;
	}

	res = pass(_arguments[0], irc.get_localhost_pass());
	clients[i]->setPassword(res);
	return;
}

bool Command::
pass(string password, string local_pass)
{
    if (password == local_pass)
	{
		utils::print_line("Correct password");
		return true;
	}
	utils::print_error(ERR_PASSWORD, "Incorrect password set");
	return false;
}
