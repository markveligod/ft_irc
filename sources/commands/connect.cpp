#include "Class.Command.hpp"
#include "Class.IRC.hpp"
#include "utils.hpp"

/*
** ====================================================================
** Команда: CONNECT
** ====================================================================
** Параметры: <target server> [<port>]
** ====================================================================
** Команда используется для попытки создания сервером нового соединения с 
** другим сервером. CONNECT-сообщением могут пользоваться только 
** IRC-операторы. Если удаленный сервер указан в строке параметров, он 
** присоединяется к <target server> на <port>.
** =====================================================================
*/

int Command::cmd_connect(IRC& irc, int fd)
{
	std::cout << arguments.size() << std::endl;
	if (!(arguments.size() == 1 || arguments.size() == 2))
		return (irc.push_mess_client(fd, ERR_NEEDMOREPARAMS));

	User* user = irc.get_user(fd);
	if (!user || !user->getMode('o'))
		return (irc.push_mess_client(fd, ERR_NOPRIVILEGES));

	// irc.add_network_ip(arguments[0]);
	// irc.add_network_port(arguments.size() == 2 ? arguments[1] : DEF_PORT);
	// irc.add_network_pass(DEF_PASS);

	// irc.create_socket_network();

	

	return 0;
}
