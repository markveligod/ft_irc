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
server_check_errors(vector<Server *> &servers,vector<Client *> &clients, int fd) const
{
	int server_el = IRC::find_fd(servers, fd);
	int client_el = IRC::find_fd(clients, fd);

	// Проверка на количство аргументов
	if (!this->check_args_number(3))
	{
		utils::print_error(ERR_NEEDMOREPARAMS, "Need more params!");
		return (ERR_NEEDMOREPARAMS);
	}

	// Проверка на то, что сервер уже зарегестрирован
	if (!this->prefix.empty() &&							  // если есть префикс
		!this->server_available(servers, this->arguments[0])) // и есть сервер с именем, поданным аргументом
	{
		utils::print_error(ERR_ALREADYREGISTRED, "Already registered! Connection closed!");
		// !!!!! и надо разорвать соединение, но как ????
		return (ERR_ALREADYREGISTRED);
	}
	if (!this->server_available(servers, this->arguments[0]) || // если такой servername уже существует
		(this->prefix.empty() &&								// если префикс пуст (то есть, к нам подключен напрямую)
		 server_el >= 0))										// и сервер с таким дескриптором уже есть
	{
		utils::print_error(ERR_ALREADYREGISTRED, "Already registered!");
		return (ERR_ALREADYREGISTRED);
	}

	// Проверка на ввод пароля
	if (this->prefix.empty() &&						// если префикс пуст
		client_el >= 0 &&							// и есть клиент с таким fd
		!this->check_password(*clients[client_el])) // и он не ввел пароль
		return 0;

	return (1);
}

int Command::
cmd_server(IRC& irc, int fd)
{
	vector<Client *>& clients	= irc.get_clients();
	vector<Server*>&  servers 	= irc.get_servers();
	vector<User *>&   users		= irc.get_users();
	int				  client_el	= IRC::find_fd(clients, fd);
	int				  check_result;


	if ((check_result = this->server_check_errors(servers, clients, fd)) != 1)
		return (check_result);


// Создаем новый сервер
	Server *new_server = new Server(fd, this->arguments[0], atoi(this->arguments[1].c_str()), this->arguments[2]);
	servers.push_back(new_server);

	std::stringstream out_message;
	out_message << new_server->getHopcount();
	utils::print_line("New server registered!\n\t  Server name: " +
					  new_server->getServerName() +
					  "\n\t  Hopcount: " + out_message.str() +
					  "\n\t  Info: " + new_server->getInfo());
	out_message.str("");

// ОТПРАВКИ СООБЩЕНИЙ

	// Если мы еще не общались с сервером, то отправляем ему свои PASS и SERVER
	if (this->prefix.empty() && clients[client_el]->getIsServer() == false)
		irc.push_cmd_queue(fd, "PASS 123\r\nSERVER " + irc.get_server_name() + " 1 info\r\n");

	// Отправка этому серверу всех наших среверов, клиентов и тд
	if (this->prefix.empty())
	{
		//irc.delete_client(fd);
		//utils::print_line("DEGUB: Client deleted (as prefix is empty)");
		for (int i = 0; i < (int)servers.size() - 1; i++)
		{
			out_message << ":" << irc.get_server_name() << " SERVER "
						<< servers[i]->getServerName() << " "
						<< (servers[i]->getHopcount() + 1) << " :"
						<< servers[i]->getInfo() << "\r\n";
			irc.push_cmd_queue(fd, out_message.str());
			out_message.str("");
		}
		for (int i = 0; i < (int)users.size(); i++)
		{
			if (users[i]->getSocketFd() != fd)
			{
				out_message << ":" << users[i]->getName() << " NICK "
							<< users[i]->getName() << " "
							<< (users[i]->getHopcount() + 1) << "\r\n";
				out_message << ":" << users[i]->getName() << " USER "
							<< users[i]->getUsername() << " "
							<< users[i]->getHostname() << " "
							<< users[i]->getServername() << " :"
							<< users[i]->getRealname() << "\r\n";
				irc.push_cmd_queue(fd, out_message.str());
				out_message.str("");
			}
		}
	}

	// Отправка сообщения SERVER всем серверам
	out_message << ":" << irc.get_server_name() <<
				  " SERVER " + this->arguments[0] <<
				  " " << (atoi(this->arguments[1].c_str()) + 1) <<
				  " " + this->arguments[2] + "\r\n";
	for (int i = 0; i < (int)servers.size(); i++)
	{
		if (i != IRC::find_fd(servers, fd) && servers[i]->getHopcount() == 1) // отправляем только напрямую подключенным серверам
			irc.push_cmd_queue(servers[i]->getSocketFd(), out_message.str());
	}

// КОНЕЦ ОТПРАВОК СООБЩЕНИЙ

	if (this->prefix.empty())
	{
		irc.delete_client(fd);
		utils::print_line("DEBUG: Client deleted (as prefix is empty)");
	}

	return (0);
}
