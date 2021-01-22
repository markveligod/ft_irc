#include "Class.Command.hpp"
#include "Class.IRC.hpp"

/*
** -----------------------------------------------------------------------
** Команда: VERSION
** -----------------------------------------------------------------------
** Параметры: [<server>]
** -----------------------------------------------------------------------
**	The VERSION message is used  to  query  the  version  of  the  server
**	program.  An optional parameter <server> is used to query the version
**	of the server program which a client is not directly connected to.
**
**	Numeric Replies:
**			ERR_NOSUCHSERVER                RPL_VERSION
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

void Command::
cmd_version(IRC &irc, int fd)
{
	vector<Client*>& 	clients		= irc.get_clients();
	vector<Server*>& 	servers		= irc.get_servers();
	int 				server_el	= IRC::find_fd(servers, fd);
	int 				client_el	= IRC::find_fd(clients, fd);
	string				client_name;
	std::stringstream	out_mess;

	if (version_check_errors(irc, fd, server_el, client_el) != 1)
		return;

	client_name = _prefix.empty() ? clients[client_el]->getName() : _prefix;

	if (_arguments.size() == 0 ||
		(_arguments.size() == 1 && _arguments[0] == irc.get_server_name()))
	{
		out_mess << VERSION << " " << irc.get_server_name() << " :Awesome IRC server";
		irc.push_cmd_queue(fd, irc.response(RPL_VERSION, client_name, "", out_mess.str()));
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
version_check_errors(IRC &irc, int fd, int server_el, int client_el)
{
	vector<Client *> &clients = irc.get_clients();
	vector<User *> &users = irc.get_users();
	vector<Server *> &servers = irc.get_servers();

	if (_prefix.size() && 
		irc.find_name(users, _prefix) < 0)
	{
		utils::print_error(0, "Invalid prefix");
		irc.push_cmd_queue(fd, "ERROR :Invalid prefix \"" + _prefix + "\"");
		return (0);
	}

	if (server_el >= 0)
	{
		if (_prefix.empty())
		{
			utils::print_error(ERR_NEEDMOREPARAMS, "Prefix is empty");
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
			utils::print_error(0, "Invalid prefix");
			irc.push_cmd_queue(fd, "ERROR :Invalid prefix \"" + _prefix + "\"");
			return (0);
		}
	}

	return (1);
}