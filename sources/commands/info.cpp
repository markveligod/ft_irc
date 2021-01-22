#include "Class.Command.hpp"
#include "Class.IRC.hpp"

void Command::
cmd_info(IRC &irc, int fd)
{
	vector<Client*>& clients	= irc.get_clients();
	vector<Server*>& servers	= irc.get_servers();
	int server_el				= IRC::find_fd(servers, fd);
	int client_el				= IRC::find_fd(clients, fd);
	string client_name;
	std::stringstream ss;

	if (info_chek_errors(irc, fd, server_el, client_el) != 1)
		return;

	client_name = _prefix.empty() ? clients[client_el]->getName() : _prefix;

	if (_arguments.size() == 0 ||
		(_arguments.size() == 1 && _arguments[0] == irc.get_server_name()))
	{
		irc.push_cmd_queue(fd, irc.response(RPL_INFO, client_name, "", ":BANDA_IRC-1.0"));
		ss << __DATE__ << " at " << __TIME__;
		irc.push_cmd_queue(fd, irc.response(RPL_INFO, client_name, ":Birth Date:", ss.str()));
		irc.push_cmd_queue(fd, irc.response(RPL_INFO, client_name, ":Online since", irc.get_statistics().getStartDate()));
		irc.push_cmd_queue(fd, irc.response(RPL_ENDOFINFO, client_name, "", ":End of INFO list"));
	}
	else if (_arguments.size() == 1)
	{
		server_el = IRC::find_name(servers, _arguments[0]);
		if (server_el < 0)
			irc.push_cmd_queue(fd, irc.response(ERR_NOSUCHSERVER, client_name, _arguments[0], ":No such server"));
		else
			irc.push_cmd_queue(servers[server_el]->getSocketFd(), (_prefix.empty() ? ":" + client_name + " " : "") + _message + "\r\n");
	}
}


int Command::
info_chek_errors(IRC &irc, int fd, int server_el, int client_el)
{
	vector<Client*>& clients	= irc.get_clients();
	vector<User*>& users		= irc.get_users();
	vector<Server*>& servers	= irc.get_servers();

	if (_prefix.size() && irc.find_name(users, _prefix) < 0)
	{
		utils::print_error(0, "Invalid _prefix");
		irc.push_cmd_queue(fd, "ERROR :Invalid _prefix \"" + _prefix + "\"");
		return (0);
	}
	if (server_el >= 0)
	{
		if (_prefix.empty())
		{
			utils::print_error(ERR_NEEDMOREPARAMS, "_prefix is empty");
			irc.push_cmd_queue(fd, irc.response(ERR_NEEDMOREPARAMS, servers[server_el]->getName(), _command, ":Syntax error"));
			return (0);
		}
		if (_arguments.size() >= 2)
		{
			utils::print_error(ERR_NEEDMOREPARAMS, "Too many parameters");
			irc.push_cmd_queue(fd, irc.response(ERR_NEEDMOREPARAMS, _prefix, _command, ":Syntax error"));
			return (0);
		}
	}
	else if (client_el >= 0)
	{
		if (_arguments.size() >= 2)
		{
			utils::print_error(ERR_NEEDMOREPARAMS, "Too many parameters");
			irc.push_cmd_queue(fd, irc.response(ERR_NEEDMOREPARAMS, clients[client_el]->getName(), _command, ":Syntax error"));
			return (0);
		}
		if (_prefix.size() && clients[client_el]->getName() != _prefix)
		{
			utils::print_error(0, "Invalid _prefix");
			irc.push_cmd_queue(fd, "ERROR :Invalid _prefix \"" + _prefix + "\"");
			return (0);
		}
	}
	return (1);
}