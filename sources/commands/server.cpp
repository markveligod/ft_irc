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
server_available(vector<Server *> &servers, string const &server_name) const
{
	std::vector<Server *>::iterator v_begin = servers.begin();
	std::vector<Server *>::iterator v_end	= servers.end();

	while (v_begin != v_end)
	{
		if ((*v_begin)->getServerName() == server_name)
			return (0);
		v_begin++;
	}
	return (1);
}

int Command::
cmd_server(IRC& irc, int fd)
{
	vector<Client *>& clients	= irc.get_clients();
	vector<Server*>&  servers 	= irc.get_servers();
	int				  server_el	= IRC::find_fd(servers, fd);
	int				  client_el	= IRC::find_fd(clients, fd);

	// Проверка на количство аргументов
	if (!this->check_args_number(3))
		return (ERR_NEEDMOREPARAMS);

	// Проверка на то, что сервер уже зарегестрирован
	if (!this->prefix.empty() &&								// если есть префикс
		!this->server_available(servers, this->arguments[0])) 	// и есть сервер с именем, поданным аргументом
	{
		utils::print_error(ERR_ALREADYREGISTRED, "Already registered! Connection closed!");
		// !!!!! и надо разорвать соединение, но как ????
		return (ERR_ALREADYREGISTRED);
	}
	if (!this->server_available(servers, this->arguments[0]) ||	// если такой servername уже существует
		(this->prefix.empty() && 								// если префикс пуст (то есть, к нам подключен напрямую)
		server_el >= 0))										// и сервер с таким дескриптором уже есть
	{
		utils::print_error(ERR_ALREADYREGISTRED, "Already registered!");
		return (ERR_ALREADYREGISTRED);
	}


	// Проверка на ввод пароля
	if (this->prefix.empty() && 					// если префикс пуст
		client_el >= 0 && 							// и есть клиент с таким fd
		!this->check_password(*clients[client_el]))	// и он не ввел пароль
		return 0;

	Server *new_server = new Server(fd, this->arguments[0], atoi(this->arguments[1].c_str()), this->arguments[2]);
	servers.push_back(new_server);
	std::stringstream ss;
	ss << new_server->getHopcount();
	utils::print_line("New server registered!\nServer name: " +
					  new_server->getServerName() +
					  "\nHopcount: " + ss.str() +
					  "\nInfo: " + new_server->getInfo());

	// отправка сообщения SERVER всем серверам
	server_el = IRC::find_fd(servers, fd);
	string out_message;
	out_message = ":" + irc.get_server_name() +
				  " SERVER " + this->arguments[0] +
				  " " + std::to_string(atoi(this->arguments[1].c_str()) + 1) +
				  " " + this->arguments[2] + "\r\n";

	//irc.push_cmd_queue(fd, ":our.server PONG :irc.example.net");
	//irc.push_cmd_queue(fd, "NICK sss 1\r\n");
	//irc.push_cmd_queue(fd, "NICK sss 1\r\n");
	for (int i = 0; i < (int)servers.size(); i++)
	{
		if (i != server_el && servers[i]->getHopcount() == 1)					// отправляем только напрямую подключенным серверам
			irc.push_cmd_queue(servers[i]->getSocketFd(), out_message);
	}

	if (this->prefix.empty())
	{
		irc.delete_client(fd);
		utils::print_line("DEGUB: Client deleted (as prefix is empty)");
	}
	return (0);
}






/*
int Command::
cmd_server(IRC& irc, int fd)
{
	vector<Client*>& vec_client 	= irc.get_clients();
	//vector<User*>& vec_users		= irc.get_users();
	vector<Server*>& vec_server 	= irc.get_servers();
	vector<Client*>::iterator temp 	= this->find_fd(vec_client, fd);

	if (!this->check_args_number(3))
		return (0);

	if (IRC::find_fd(vec_server, fd) >= 0)
	{
		utils::print_error(ERR_FINDFD, "Already registered!");
		return (ERR_ALREADYREGISTRED);
	}

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
	utils::print_line("New server registered! " + new_server->getServerName());

	// отправка сообщения SERVER всем серверам
	int j = IRC::find_fd(vec_server, fd);
	string out_message;
	std::stringstream ss;
	ss << atoi(this->arguments[1].c_str()) + 1;
	out_message = ":" + irc.get_server_name() +
				  " SERVER " + this->arguments[0] +
				  " " + ss.str() + 
				  " " + this->arguments[2] + "\r\n";
	for (int i = 0; i < (int)vec_server.size(); i++)
	{
		if (i != j)
			irc.push_cmd_queue(vec_server[i]->getSocketFd(), out_message);
	}

	for (size_t i = 0; i < vec_users.size(); i++)
	{
		string temp_str = "NICK " + vec_users[i]->getNickname() + " " + utils::int_to_str(vec_users[i]->getHopcount()) + "\r\nUSER " + vec_users[i]->getUsername() + " " + vec_users[i]->getHostname() + " " + vec_users[i]->getServername() + " " + vec_users[i]->getRealname() + "\r\n";
		// irc.push_cmd_queue(new_server->getSocketFd(), temp_str);
	}
	return 0;
}*/
