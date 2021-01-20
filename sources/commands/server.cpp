#include "main.hpp"
#include "Class.Command.hpp"

/*
** =============================================================
** Команда: SERVER 
** =============================================================
** Параметры: <servername> <hopcount> <info>
** =============================================================
** cmd_server - добавляет в вектор серверов => текущий сервер
** =============================================================
*/

int Command::
cmd_server(IRC& irc, int fd)
{
	vector<Client *>& clients	= irc.get_clients();
	vector<Server*>&  servers 	= irc.get_servers();
	vector<User *>&   users		= irc.get_users();
	int				  client_el	= IRC::find_fd(clients, fd);
	int				  server_el = IRC::find_fd(servers, fd);
	int				  error;
	Server*			  new_server;

	if ((error = this->server_check_errors(irc, fd)) != 1)
		return (error);


// Создаем новый сервер
	if (server_el < 0)
	{
		new_server = new Server(fd, this->arguments[0], atoi(this->arguments[1].c_str()), this->arguments[2]);
		new_server->getStatistics() = clients[client_el]->getStatistics();
	}
	else
		new_server = new Server(fd, this->arguments[0], atoi(this->arguments[1].c_str()), this->arguments[3]);
	servers.push_back(new_server);

// Вывод сообщения о созданном сервере
	std::stringstream out_message;
	out_message << "New server registered!"
				<< "\n\t  Server name: " << new_server->getName()
				<< "\n\t  Hopcount: " << new_server->getHopcount()
				<< "\n\t  Info: " << new_server->getInfo();
	utils::print_line(out_message.str());
	out_message.str("");


// ОТПРАВКИ СООБЩЕНИЙ

	// Если сообщение от клиента (еще не зареганного сервера)
	if (server_el < 0)
	{
		if (clients[client_el]->getIsServer() == false)
			irc.push_cmd_queue(fd, "PASS 123\r\nSERVER " + irc.get_server_name() + " 1 info\r\n");
		for (int i = 0; i < (int)servers.size() - 1; i++)
		{
			out_message << ":" << irc.get_server_name() << " SERVER "
						<< servers[i]->getName() << " "
						<< (servers[i]->getHopcount() + 1) << " "
						<< 0 << " :"
						<< servers[i]->getInfo() << "\r\n";
			irc.push_cmd_queue(fd, out_message.str());
			out_message.str("");
		}
		for (int i = 0; i < (int)users.size(); i++)
		{
			if (users[i]->getSocketFd() != fd)
			{
				out_message << "NICK "
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
		new_server->setBuffer(clients[client_el]->getBuffer());
		//irc.delete_client(fd);
		utils::print_line("DEBUG: Client deleted (as it is a new connection)");
	}

	out_message << ":" << irc.get_server_name() <<
				  " SERVER " + new_server->getName() <<
				  " " << (new_server->getHopcount() + 1) << " " <<
				  (new_server->getHopcount() + 1) << " " << new_server->getInfo();
	irc.forward_to_servers(fd, out_message.str());

	return (0);
}

int Command::
server_available(vector<Server *> &servers, string const &server_name) const
{
	std::vector<Server *>::iterator v_begin = servers.begin();
	std::vector<Server *>::iterator v_end	= servers.end();

	while (v_begin != v_end)
	{
		if (is_equal((*v_begin)->getName(), server_name))
			return (0);
		v_begin++;
	}
	return (1);
}

int Command::
server_check_errors(IRC& irc, int fd) const
{
	vector<Client *> &clients = irc.get_clients();
	vector<Server *> &servers = irc.get_servers();
	int server_el = IRC::find_fd(servers, fd);
	int client_el = IRC::find_fd(clients, fd);

	// Если сообщение непонятно от кого
	if (server_el < 0 && client_el < 0)
	{
		utils::print_error(0, "Who the hell sent it???");
		irc.push_cmd_queue(fd, "ERROR :SERVER DEBUG who are you?\r\n");
		return (0);
	}

	if (server_el >= 0 && arguments.size() != 4)
	{
		utils::print_error(ERR_NEEDMOREPARAMS, "Invalid number of parameters");
		irc.push_cmd_queue(fd, irc.response_2(ERR_NEEDMOREPARAMS, fd, "* SERVER", ERR_NEEDMOREPARAMS_MESS));
		return (ERR_NEEDMOREPARAMS);
	}

	if (server_el < 0 && arguments.size() != 3)
	{
		utils::print_error(ERR_NEEDMOREPARAMS, "Invalid number of parameters");
		irc.push_cmd_queue(fd, irc.response_2(ERR_NEEDMOREPARAMS, fd, "* SERVER", ERR_NEEDMOREPARAMS_MESS));
		return (ERR_NEEDMOREPARAMS);
	}

	// Если сообщение от клиента, который уже ввел NICK
	if (server_el < 0 && !clients[client_el]->getName().empty())
	{
		utils::print_error(0, "Registered client sent SERVER command. Why?");
		irc.push_cmd_queue(fd, "ERROR :SERVER DEBUG You are a client, not a server!\r\n");
		return (0);
	}

	// Если сообщение от уже зарегестрированного сервера и без префикса
	if (server_el >= 0 && prefix.empty())
	{
		utils::print_error(0, "SERVER command without prefix");
		irc.push_cmd_queue(fd, "ERROR :SERVER command without prefix\r\n");
		return (0);
	}

	// Если префикса нет среди серверов
	if (server_el >= 0 && !this->prefix.empty() && this->server_available(servers, prefix))
	{
		utils::print_error(0, "Invalid prefix");
		irc.push_cmd_queue(fd, "ERROR :Invalid prefix \"" + prefix + "\"\r\n");
		return (0);
	}

	// Проверка на то, что сервер уже зарегестрирован
	if (!this->prefix.empty() &&							  // если есть префикс
		!this->server_available(servers, this->arguments[0])) // и есть сервер с именем, поданным аргументом
	{
		utils::print_error(ERR_ALREADYREGISTRED, "Already registered! Connection closed!");
		// !!!!! и надо разорвать соединение, но как ????
		return (ERR_ALREADYREGISTRED);
	}
	if (!this->server_available(servers, this->arguments[0]))// если такой servername уже существует									// и сервер с таким дескриптором уже есть
	{
		utils::print_error(ERR_ALREADYREGISTRED, "Already registered!");
		return (ERR_ALREADYREGISTRED);
	}

	// Проверка на ввод пароля
	if (server_el < 0 &&							// Если пришло от клиента
		!this->check_password(*clients[client_el])) // и он не ввел пароль
	{
		irc.push_cmd_queue(fd, "ERROR :Enter valid PASS\r\n");
		return 0;
	}
		

	return (1);
}

