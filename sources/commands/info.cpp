#include "Class.Command.hpp"
#include "Class.IRC.hpp"

int Command::
cmd_info(IRC &irc, int fd)
{
	vector<Client*>& clients	= irc.get_clients();
	vector<Server*>& servers	= irc.get_servers();
	int server_el				= IRC::find_fd(servers, fd);
	int client_el				= IRC::find_fd(clients, fd);
	string client_name;
	std::stringstream ss;

	if (info_chek_errors(irc, fd, server_el, client_el) != 1)
		return (0);

	client_name = prefix.empty() ? clients[client_el]->getName() : prefix;

	if (arguments.size() == 0 ||
		(arguments.size() == 1 && arguments[0] == irc.get_server_name()))
	{
		irc.push_cmd_queue(fd, irc.response_3(RPL_INFO, client_name, "", ":BANDA_IRC-1.0"));
		ss << __DATE__ << " at " << __TIME__;
		irc.push_cmd_queue(fd, irc.response_3(RPL_INFO, client_name, ":Birth Date:", ss.str()));
		irc.push_cmd_queue(fd, irc.response_3(RPL_INFO, client_name, ":Online since", irc.get_statistics().getStartDate()));
		irc.push_cmd_queue(fd, irc.response_3(RPL_ENDOFINFO, client_name, "", ":End of INFO list"));
	}
	else if (arguments.size() == 1)
	{
		server_el = IRC::find_name(servers, arguments[0]);
		if (server_el < 0)
			irc.push_cmd_queue(fd, irc.response_3(ERR_NOSUCHSERVER, client_name, arguments[0], ":No such server"));
		else
			irc.push_cmd_queue(servers[server_el]->getSocketFd(), (prefix.empty() ? ":" + client_name + " " : "") + message + "\r\n");
	}

	return (1);
}


int Command::
info_chek_errors(IRC &irc, int fd, int server_el, int client_el)
{
	vector<Client*>& clients	= irc.get_clients();
	vector<User*>& users		= irc.get_users();
	vector<Server*>& servers	= irc.get_servers();

	if (prefix.size() && irc.find_name(users, prefix) < 0)
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
		if (arguments.size() >= 2)
		{
			utils::print_error(ERR_NEEDMOREPARAMS, "Too many parameters");
			irc.push_cmd_queue(fd, irc.response_3(ERR_NEEDMOREPARAMS, prefix, command, ":Syntax error"));
			return (0);
		}
	}
	else if (client_el >= 0)
	{
		if (arguments.size() >= 2)
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