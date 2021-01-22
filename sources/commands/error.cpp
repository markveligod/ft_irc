#include "Class.Command.hpp"
#include "Class.IRC.hpp"
#include "utils.hpp"

/*
** ====================================================================
** Команда: ERROR
** ====================================================================
** Параметры: [message]
** ====================================================================
** Команда ERROR предназначена для использования серверами, когда они 
** сообщают о серьезных или смертельных ошибках IRC-операторам. Сообщение 
** так же может быть отправлена с одного сервера на другой, но это не 
** сможет быть подтверждено с любых нормальных неизвестных клиентов. 
** ERROR используется для сообщения ошибок, которые случаются только при 
** соединениях класса сервер-к-серверу. Сообщение отсылается на сервер с 
** другого конца соединения (который оправляет его всем IRC-операторам, 
** подсоединенным к нему) и всем операторам текущего соединения. Оно не 
** может быть отослано каким-либо другим серверам, если оно не получено с 
** сервера.
** Когда сервер отсылает полученное ERROR-сообщение к своим операторам, 
** сообщение следует поместить в сообщение NOTICE, сообщающее, что клиент 
** не может отвечать на ошибку.  
** =====================================================================
*/

void Command::
cmd_error(IRC& irc, int fd)
{
	if (!irc.is_server(fd))
		return;

	string notice = ":" + irc.get_server_name() + " " + "NOTICE" + " ";
	string error = " :ERROR :" + ((!_arguments.empty()) ? _arguments[0] : "") + "\r\n";
	vector<User*> users = irc.get_users();

	for (size_t i = 0; i < users.size(); i++)
	{
		if (users[i]->getMode('o') && users[i]->getHopcount() == 0)
			irc.push_cmd_queue(fd, notice + users[i]->getName() + error);
	}

	Server* quit_server = (!_prefix.empty())
							? irc.get_server(_prefix)
							: irc.get_server(fd);

	if (!quit_server) return;

	string quit_server_name = quit_server->getName();

	for (size_t i = 0; i < users.size(); i++)
	{
		if ((quit_server->getHopcount() == 1								// если удаяемы сервер подключен к нам напрямую
				&& users[i]->getSocketFd() == quit_server->getSocketFd())	// то удаляем всех пользователей в цепочке серверов (по fd)
			|| users[i]->getServername() == quit_server_name)
			irc.delete_user(users[i]);										// удаляем пользователя из всех каналов, из _users и из _clients 
	}

	irc.close_connection(quit_server);
}
