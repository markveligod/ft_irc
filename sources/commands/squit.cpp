#include "Class.Command.hpp"
#include "Class.IRC.hpp"
#include "utils.hpp"

/*
** ====================================================================
** Команда: SQUIT
** ====================================================================
** Параметры: <server> <comment> 
** ====================================================================
** Сообщение SQUIT требуется для указания мертвых или вышедших серверов. 
** Если сервер желает оборвать соединение с другим сервером, он должен 
** послать сообщение SQUIT другому серверу, используя имя другого сервера 
** в качестве параметра server. 
** Эта команда так же доступна IRC-операторам для помощи в сохранеии сети 
** соединений IRC-серверов в порядке. IRC-операторы могут так же послать 
** SQUIT для удаленных серверных соединений. В этом случае, SQUID будет 
** парситься каждым сервером, находящимся между IRC-оператором и удаленным 
** сервером. 
** Параметр <comment> обеспечивается всеми операторами, которые запускают 
** SQUIT для удаленных серверов (которые не присоединены к серверу, 
** который хотят выключить), так что все операторы знают причины этого 
** действия. 
** Один из серверов, которые находятся на другой стороне соединения, будет 
** закрыт по требованию, высланным сообщением SQUID (ко всем другим 
** соединениям) дл остальных серверво, которые рассматриваются как линки. 
** Подобным образом, SQUIT может быть послана другим серверам, находящимся 
** в сети ради клиентов. В дополнение к этому, все члены канала, который 
** разбило сплитом, может послать SQUIT-сообщение. 
** Если соединение сервера закрыто преждевременно (т.е сервер на другом 
** конце соединения разорвал линк), сервер, который засек этот разрыв 
** соединения, информирует всю сеть о том, что соединение закрыто и 
** показывает поле <comment>, обьясняя причину рассоединения. 
** =====================================================================
*/

int Command::
cmd_squit(IRC& irc, int fd)
{
    std::vector<Server*>& servers = irc.get_servers();
    std::vector<User*>& users = irc.get_users();
	User* usr = (!prefix.empty()) ? irc.get_user(prefix) : irc.get_user(fd);
	if (!usr) return 1;

	if (!usr->getMode('o'))
		return (irc.push_mess_client(fd, ERR_NOPRIVILEGES));

	if (!this->check_args_number(2))
		return (irc.push_mess_client(fd, ERR_NEEDMOREPARAMS));

	int pos;
	if ((pos = irc.find_name(irc.get_servers(), arguments[0])) < 0)
		return (irc.push_mess_client(fd, ERR_NOSUCHSERVER));

	string quit_server_name = arguments[0];		// имя отключаемого сервера
	Server* srvr = irc.get_server(arguments[0]);// отлючаемый сервер

	for (size_t i = 0; i < users.size(); i++)
	{
		if ((srvr->getHopcount() == 1								// если удаяемы сервер подключен к нам напрямую
				&& users[i]->getSocketFd() == srvr->getSocketFd())	// то удаляем всех пользователей в цепочке серверов (по fd)
			|| users[i]->getServername() == quit_server_name)
			irc.delete_user(users[i]);								// удаляем пользователя из всех каналов, из _users и из _clients 
	}

	string quit_msg1 = ":" + irc.get_server_name() +
								" WALLOPS :Received SQUIT " +
								arguments[0] +
								" from " +
								usr->getName() + ": " +
								arguments[1];

	irc.forward_to_servers(fd, quit_msg1);

	string quit_msg2 = ":" + quit_server_name +
								" SQUIT " +
								quit_server_name + " :" +
								arguments[1] +
								" (SQUIT from " +
								usr->getName() + ")";

	irc.forward_to_servers(fd, quit_msg2);

	irc.close_connection(servers[pos]);
	return (0);
}

// squit ft_irc.net/1084 :uhodi sorry  