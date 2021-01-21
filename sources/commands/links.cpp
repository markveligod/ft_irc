#include "Class.Command.hpp"
#include "Class.IRC.hpp"

/*
** -----------------------------------------------------------------------
** Команда: LINKS
** -----------------------------------------------------------------------
** Параметры: [[<remote server>] <server mask>]
** -----------------------------------------------------------------------
**	With LINKS, a user can list all servers which are known by the server
**  answering the query.  The returned list of servers must match the
**  mask, or if no mask is given, the full list is returned.
**
**  If <remote server> is given in addition to <server mask>, the LINKS
**  command is forwarded to the first server found that matches that name
**  (if any), and that server is then required to answer the query.
**
**	Numeric Replies:
**			ERR_NOSUCHSERVER      RPL_LINKS       RPL_ENDOFLINKS
** -----------------------------------------------------------------------
**	RPL_VERSION - 351
**    	    "<version>.<debuglevel> <server> :<comments>"
**
**    	  - Reply by the server showing its version details.
**          The <version> is the version of the software being
**		    used (including any patchlevel revisions) and the
**          <debuglevel> is used to indicate if the server is
**          running in "debug mode".
**          The "comments" field may contain any comments about
**          the version or further version details.
** -----------------------------------------------------------------------
*/
vector<Server *> Command::
find_by_mask(string const &str_mask, IRC &irc, bool &our_server)
{
	int										curr		= 0;
	size_t									our_server_position;
	char *									tok;
	char *									old_tok;
	char *									mask 		= new char[str_mask.length() + 1];
	size_t									pos;
	Server *								server;
	vector<Server *> & 						servers		= irc.get_servers();
	vector<Server *>						mask_servers;
	vector<pair<size_t, int> >				mask_positions;
	vector<Server *>::iterator				it;
	vector<pair<size_t, int> >::iterator	pos_it;

	our_server = true;
	strcpy(mask, str_mask.c_str());
	tok = strtok(mask, "*");

	if (str_mask[0] != '*')											// Если до первой * что-то есть
	{
		for (it = servers.begin(); it != servers.end(); it++)		// ищем по всем серверам
		{
			server = (*it);
			if (server->getName().find(tok) == 0)					// проверяем, подходит ли по маске (= 0 так как это до первой *, значит так должно начинаться начало имени)
			{
				mask_servers.push_back(server);						// если да, добавляем в вектор подходящих серверов
				mask_positions.push_back(std::make_pair<size_t, int>(strlen(tok), 0));
			}
		}
		if (irc.get_server_name().find(tok) != 0)					// проверяем, подходит ли наш сервер
			our_server = false;										// если нет, ставим false
		else
			our_server_position = strlen(tok);
		if ((tok = strtok(NULL, "*")) == NULL) // берем следующий кусочек
			return (mask_servers);
	}
	else															// Если до первой * ничего нет, список подходящих серверов - это все сервера
	{
		mask_servers = servers;
		for (int i = 0; i < (int)servers.size(); i++)
			mask_positions.push_back(std::make_pair<size_t, int>(0, 0));
	}

	while (tok != NULL)
	{
		it = mask_servers.begin();
		pos_it = mask_positions.begin();
		curr++;
		while (it != mask_servers.end())
		{
			server = (*it);
			if (((*pos_it).second != curr) && 
				((pos = server->getName().find(tok)) == std::string::npos ||
				pos < (*pos_it).first))
			{
				mask_servers.erase(it);
				mask_positions.erase(pos_it);
				it = mask_servers.begin();
				pos_it = mask_positions.begin();
			}
			else
			{
				(*pos_it).first = pos + strlen(tok);
				(*pos_it).second = curr;
				it++;
				pos_it++;
			}
		}
		if (our_server == true &&
			irc.get_server_name().find(tok) == std::string::npos)	// проверяем, подходит ли наш сервер
			our_server = false;										// если нет, ставим false
		old_tok = tok;
		if ((tok = strtok(NULL, "*")) == NULL)						// берем следующий кусочек
		{															// если этот кусочек последний
			if (str_mask[str_mask.size() - 1] == '*')				// если последний элемент * то всё ок
				break;
			it = mask_servers.begin();
			pos_it = mask_positions.begin();
			curr++;
			while (it != mask_servers.end())
			{
				server = (*it);
				pos = server->getName().find(old_tok);
				if (((*pos_it).second != curr) &&
					(pos + strlen(old_tok)) != server->getName().size())
				{
					mask_servers.erase(it);
					mask_positions.erase(pos_it);
					it = mask_servers.begin();
					pos_it = mask_positions.begin();
				}
				else
				{
					(*pos_it).second = curr;
					it++;
					pos_it++;
				}
			}
			if (our_server == true &&
				irc.get_server_name().find(old_tok) + strlen(old_tok) != irc.get_server_name().size())
				our_server = false;
		}
	}


	// DEBUG
	std::cout << "\n\nMASK SERVERS " << mask_servers.size() << "\n";
	for (it = mask_servers.begin(); it != mask_servers.end(); it++)
	{
		std::cout << (*it)->getName() << std::endl;
	}

	delete [] mask;
	return (mask_servers);
}

vector<Server *> Command::
links_find(IRC &irc,
		   int fd,
		   string const & mask, 
		   string const & our_servername,
		   bool & our_server)
{
	vector<Server *> &servers   = irc.get_servers();
	int 			  server_el	= IRC::find_fd(servers, fd);
	vector<Server *>  output_servers;

	if (mask.find('*') == std::string::npos)
	{
		server_el = IRC::find_name(servers, mask);
		if (server_el >= 0)
			output_servers.push_back(servers[server_el]);
		if (mask != our_servername)
			our_server = false;
	}
	else
		output_servers = find_by_mask(mask, irc, our_server);

	return (output_servers);
}

int Command::
cmd_links(IRC &irc, int fd)
{
	vector<Client*>& 	clients		= irc.get_clients();
	vector<Server*>& 	servers		= irc.get_servers();
	int 				server_el	= IRC::find_fd(servers, fd);
	int 				client_el	= IRC::find_fd(clients, fd);

	string				client_name;
	std::stringstream	out_mess;
	bool				our_server = true;
	vector<Server*>		output_servers;
	Server *			remote_server = NULL;

	if (links_check_errors(irc, fd, server_el, client_el) != 1)
		return (0);

	client_name = prefix.empty() ? clients[client_el]->getName() : prefix;

	if (arguments.size() == 0)
		output_servers = irc.get_servers();

	else if (arguments.size() == 1)
		output_servers = links_find(irc, fd, arguments[0], irc.get_server_name(), our_server);

	else if (arguments.size() == 2)
	{
		if (arguments[0].find('*') == std::string::npos)
		{
			server_el = IRC::find_name(servers, arguments[0]);
			if (server_el >= 0)
				remote_server = servers[server_el];
			if (arguments[0] != irc.get_server_name())
				our_server = false;
		}
		else if (find_by_mask(arguments[0], irc, our_server).size())
				remote_server = find_by_mask(arguments[0], irc, our_server)[0];
		if (remote_server == NULL && our_server == false)
		{
			irc.push_cmd_queue(fd, irc.response_3(ERR_NOSUCHSERVER, clients[client_el]->getName(), arguments[0], ":No such server"));
			return (0);
		}
		else if (remote_server == NULL && our_server == true)
			output_servers = links_find(irc, fd, arguments[1], irc.get_server_name(), our_server);
		else if (remote_server != NULL)
		{
			out_mess << ":" << client_name
					 << " LINKS "
					 << remote_server->getName() << " "
					 << arguments[1] << "\r\n";
			irc.push_cmd_queue(remote_server->getSocketFd(), out_mess.str());
			return (0);
		}
	}
	

	for (size_t i = 0; i < output_servers.size(); i++)
	{
		if (output_servers[i]->getHopcount() != 1)
			out_mess << servers[IRC::find_fd(servers, output_servers[i]->getSocketFd())]->getName();
		else
			out_mess << irc.get_server_name();
		out_mess << " :" << output_servers[i]->getHopcount() << " " << output_servers[i]->getInfo();
		irc.push_cmd_queue(fd, irc.response_3(RPL_LINKS, client_name, output_servers[i]->getName(), out_mess.str()));
		out_mess.str("");
	}
	if (our_server)
	{
		out_mess << irc.get_server_name() << " :0 " << INFO;
		irc.push_cmd_queue(fd, irc.response_3(RPL_LINKS, client_name, irc.get_server_name(), out_mess.str()));
		out_mess.str("");
	}
	irc.push_cmd_queue(fd, irc.response_3(RPL_ENDOFLINKS, client_name, "*", ":End of LINKS list"));

	return (0);
}

int Command::
links_check_errors(IRC &irc, int fd, int server_el, int client_el)
{
	vector<Client *> &	clients = irc.get_clients();
	vector<User *> &	users	= irc.get_users();
	vector<Server *> &	servers	= irc.get_servers();

	if (prefix.size() && 
		irc.find_name(users, prefix) < 0)
	{
		utils::print_error(0, "Invalid prefix");
		irc.push_cmd_queue(fd, "ERROR :Invalid prefix \"" + prefix + "\"");
		return (0);
	}

	if (server_el >= 0)
	{
		if (prefix.empty())
		{
			utils::print_error(ERR_NEEDMOREPARAMS, "Prefix is empty");
			irc.push_cmd_queue(fd, irc.response_3(ERR_NEEDMOREPARAMS, servers[server_el]->getName(), command, ":Syntax error"));
			return (0);
		}
		if (arguments.size() > 2)
		{
			utils::print_error(ERR_NEEDMOREPARAMS, "Too many parameters");
			irc.push_cmd_queue(fd, irc.response_3(ERR_NEEDMOREPARAMS, prefix, command, ":Syntax error"));
			return (0);
		}
	}

	else if (client_el >= 0)
	{
		if (arguments.size() > 2)
		{
			utils::print_error(ERR_NEEDMOREPARAMS, "Too many parameters");
			irc.push_cmd_queue(fd, irc.response_3(ERR_NEEDMOREPARAMS, clients[client_el]->getName(), command, ":Syntax error"));
			return (0);
		}
		if (prefix.size() && clients[client_el]->getName() != prefix)
		{
			utils::print_error(0, "Invalid prefix");
			irc.push_cmd_queue(fd, "ERROR :Invalid prefix \"" + prefix + "\"");
			return (0);
		}
	}

	return (1);
}
