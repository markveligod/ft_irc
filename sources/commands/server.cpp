#include "main.hpp"
#include "Class.Command.hpp"

/*
** =============================================================
** Команда: SERVER 
** =============================================================
** Параметры: <servername> <hopcount> <info>
** cmd_server - добавляет в вектор серверов => текущий сервер
** =============================================================
*/

int Command::
cmd_server(IRC& irc, int fd)
{
	vector<Client*>& vec_client 	= irc.get_clients();
	vector<User*>& vec_users		= irc.get_users();
	vector<Server*>& vec_server 	= irc.get_servers();
	vector<Client*>::iterator temp 	= this->find_fd(vec_client, fd);

	if (!this->check_args_number(3))
		return (0);

	if (IRC::find_fd(vec_server, fd) >= 0)
		return (ERR_ALREADYREGISTRED);

	if (temp == vec_client.end())
	{
		utils::print_error(ERR_FINDFD, "FD don't find in vector!");
		return 0;
	}

	if (!this->check_password(**temp))
		return 0;

	Server* new_server = new Server((*temp)->getSocketFd(), this->arguments[0], atoi(this->arguments[1].c_str()), this->arguments[2]);
	vec_server.push_back(new_server);
	irc.delete_client(fd);

	for (size_t i = 0; i < vec_users.size(); i++)
	{
		string temp_str = "NICK " + vec_users[i]->getNickname() + " " + utils::int_to_str(vec_users[i]->getHopcount()) + "\r\nUSER " + vec_users[i]->getUsername() + " " + vec_users[i]->getHostname() + " " + vec_users[i]->getServername() + " " + vec_users[i]->getRealname() + "\r\n";
		// irc.push_cmd_queue(new_server->getSocketFd(), temp_str);
	}
	return 0;
}
