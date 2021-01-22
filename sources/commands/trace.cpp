#include "Class.Command.hpp"
#include "Class.IRC.hpp"

/*
** -----------------------------------------------------------------------
**  Команда: TRACE
** -----------------------------------------------------------------------
**  Parameters: [<server>]
** -----------------------------------------------------------------------
**	 TRACE command is used to find the route to specific server.  Each
**   server that processes this message must tell the sender about it by
**   sending a reply indicating it is a pass-through link, forming a chain
**   of replies similar to that gained from using "traceroute".  After
**   sending this reply back, it must then send the TRACE message to the
**   next server until given server is reached.  If the <server> parameter
**   is omitted, it is recommended that TRACE command send a message to
**   the sender telling which servers the current server has direct
**   connection to.
**
**   If the destination given by "<server>" is an actual server, then the
**   destination server is required to report all servers and users which
**   are connected to it, although only operators are permitted to see
**   users present.  If the destination given by <server> is a nickname,
**   they only a reply for that nickname is given.
**
**
**	Numeric Replies:
**			ERR_NOSUCHSERVER                RPL_TRACELINK
**          RPL_TRACECONNECTING             RPL_TRACEHANDSHAKE
**          RPL_TRACEUNKNOWN                RPL_TRACEOPERATOR
**          RPL_TRACEUSER                   RPL_TRACESERVER
**          RPL_TRACESERVICE                RPL_TRACENEWTYPE
**          RPL_TRACECLASS
** -----------------------------------------------------------------------
*/
void Command::
cmd_trace(IRC &irc, int fd)
{
	vector<Client*>& 	clients		= irc.get_clients();
	vector<Server*>& 	servers		= irc.get_servers();
	int 				client_el	= IRC::find_fd(clients, fd);
	string				client_name;
	int					server_dest;
	int					server_parent;
	std::stringstream	out_mess;

	if (trace_check_errors(irc, fd, client_el) != 1)
		return;

	client_name = _prefix.empty() ? clients[client_el]->getName() : _prefix;

	if (_arguments.size() == 0 ||
		(_arguments.size() == 1 && _arguments[0] == irc.get_server_name()))
	{
		for (size_t i = 0; i < servers.size(); i++)
		{
			if (servers[i]->getHopcount() == 1)
			{
				out_mess << 1 << " 0S " 
						 << "0C " 
						 << servers[i]->getName() 
						 << " *!*@" << irc.get_server_name() 
						 << " :VF";
				irc.push_cmd_queue(fd, irc.response(RPL_TRACESERVER, client_name, "Server", out_mess.str()));
				out_mess.str("");
			}
		}
		out_mess << VERSION << " :End of TRACE";
		irc.push_cmd_queue(fd, irc.response(262, client_name, irc.get_server_name(), out_mess.str()));
		out_mess.str("");
	}
	else if (_arguments.size() == 1)
	{
		server_dest = IRC::find_name(servers, _arguments[0]);
		if (server_dest < 0)
		{
			irc.push_cmd_queue(fd, irc.response(ERR_NOSUCHSERVER, client_name, _arguments[0], ":No such server"));
			return;
		}
		server_parent = IRC::find_fd(servers, servers[server_dest]->getSocketFd());
		out_mess << VERSION << " "
				 << _arguments[0] << " "
				 << servers[server_parent]->getName() << " VF "
				 << servers[server_parent]->getStatistics().getWorkingTime() << " "
				 << servers[server_parent]->getStatistics().getQueueCount() << " 0";
		irc.push_cmd_queue(fd, irc.response(RPL_TRACELINK, client_name, "Link", out_mess.str()));
		out_mess.str("");
		out_mess << ":" << client_name << " TRACE " << _arguments[0] << "\r\n";
		irc.push_cmd_queue(servers[server_dest]->getSocketFd(), out_mess.str());
		out_mess.str("");
	}
}


int Command::
trace_check_errors(IRC &irc, int fd, int client_el)
{
	vector<Client *> &	clients		= irc.get_clients();
	vector<User *> &	users		= irc.get_users();
	vector<Server *> &	servers		= irc.get_servers();
	int					server_el 	= irc.find_fd(servers, fd);

	if (_prefix.size() && 
		irc.find_name(users, _prefix) < 0)
	{
		utils::print_error(0, "Invalid prefix");
		irc.push_cmd_queue(fd, "ERROR :Invalid prefix \"" + _prefix + "\"");
		return 0;
	}

	if (server_el >= 0)
	{
		if (_prefix.empty())
		{
			utils::print_error(ERR_NEEDMOREPARAMS, "prefix is empty");
			irc.push_cmd_queue(fd, irc.response(ERR_NEEDMOREPARAMS, servers[server_el]->getName(), _command, ":Syntax error"));
			return 0;
		}
		if (_arguments.size() != 1)
		{
			utils::print_error(ERR_NEEDMOREPARAMS, "Invalid number of parameters");
			irc.push_cmd_queue(fd, irc.response(ERR_NEEDMOREPARAMS, _prefix, _command, ":Syntax error"));
			return 0;
		}
	}

	else if (client_el >= 0)
	{
		if (_arguments.size() > 1)
		{
			utils::print_error(ERR_NEEDMOREPARAMS, "Too many parameters");
			irc.push_cmd_queue(fd, irc.response(ERR_NEEDMOREPARAMS, clients[client_el]->getName(), _command, ":Syntax error"));
			return 0;
		}
		if (_prefix.size() && clients[client_el]->getName() != _prefix)
		{
			utils::print_error(0, "Invalid prefix");
			irc.push_cmd_queue(fd, "ERROR :Invalid prefix \"" + _prefix + "\"");
			return 0;
		}
	}
	return 1;
}