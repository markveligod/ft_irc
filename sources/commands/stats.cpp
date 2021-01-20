#include "Class.Command.hpp"
#include "Class.IRC.hpp"

/*
** ====================================================================
** Команда: STATS 
** ====================================================================
** Параметры: [ <query> [ <target> ] ]
** ====================================================================
** The stats command is used to query statistics of certain server.  If
** <query> parameter is omitted, only the end of stats reply is sent
** back.
**
** A query may be given for any single letter which is only checked by
** the destination server and is otherwise passed on by intermediate
** servers, ignored and unaltered.
**
** Wildcards are allowed in the <target> parameter.
** Except for the ones below, the list of valid queries is
** implementation dependent.
**
** The standard queries below SHOULD be supported by the server:

    l - returns a list of the server's connections, showing how
        long each connection has been established and the
        traffic over that connection in Kbytes and messages for
        each direction;
    m - returns the usage count for each of commands supported
        by the server; commands for which the usage count is
        zero MAY be omitted;
    o - returns a list of configured privileged users, operators;
    u - returns a string showing how long the server has been up.

** It is also RECOMMENDED that client and server access configuration be
** published this way.
** =====================================================================
*/

void Command::
stats_u(IRC& irc, int fd, Client* client)
{
	char buffer[20];
	struct tm *work_time = irc.get_statistics().getWorkingTime_t();
	std::stringstream out_mess;

	work_time->tm_hour -= 3;
	strftime(buffer, 20, "days %X", work_time);
	out_mess << ":Server Up " << work_time->tm_yday << " " << buffer;
	irc.push_cmd_queue(fd, irc.response_3(RPL_STATSCOMMANDS, client->getName(), "", out_mess.str()));
}

void Command::
stats_m(IRC& irc, int fd, Client* client)
{
	map<string, pair<int, unsigned long> >::iterator it		= irc.get_statistics().getMapCmd().begin();
	map<string, pair<int, unsigned long> >::iterator it_end	= irc.get_statistics().getMapCmd().end();
	map<string, pair<int, unsigned long> >::iterator client_command;
	std::stringstream out_mess;
	int curr_count;

	while (it != it_end)
	{
		if ((*it).second.first != 0)
		{
			client_command = client->getStatistics().getMapCmd().find((*it).first);
			if (client_command != client->getStatistics().getMapCmd().end())
				curr_count = (*client_command).second.first;
			else
				curr_count = 0;

			out_mess << curr_count << " "
					 << (*it).second.second << " "
					 << (*it).second.first - curr_count;
			irc.push_cmd_queue(fd, irc.response_3(RPL_STATSCOMMANDS, client->getName(), (*it).first, out_mess.str()));
			out_mess.str("");
		}
		it++;
	}
}

void Command::
stats_l(IRC& irc, int fd, Client* client)
{
	vector<Server *> & servers		= irc.get_servers();
	vector<User *> & users 			= irc.get_users();
	int	user_el 					= irc.find_name(users, client->getName());
	std::stringstream out_mess;
	string fullname;

	for (int i = 0; i != (int)servers.size(); i++)
	{
		if (servers[i]->getHopcount() == 1)
		{
			out_mess << servers[i]->getStatistics().getQueueCount() << " "
					 << servers[i]->getStatistics().getSentCount() << " "
					 << servers[i]->getStatistics().getSentKBytes() << " "
					 << servers[i]->getStatistics().getRecvCount() << " "
					 << servers[i]->getStatistics().getRecvKBytes() << " "
					 << servers[i]->getStatistics().getWorkingTime();
			irc.push_cmd_queue(fd, irc.response_3(211, client->getName(), servers[i]->getName(), out_mess.str()));
			out_mess.str("");
		}
	}
	if (client->getHopcount() == 0)
	{
		out_mess << client->getStatistics().getQueueCount() << " "
				<< client->getStatistics().getSentCount() << " "
				<< client->getStatistics().getSentKBytes() << " "
				<< client->getStatistics().getRecvCount() << " "
				<< client->getStatistics().getRecvKBytes() << " "
				<< client->getStatistics().getWorkingTime();
		fullname = client->getName() + "!~" + (user_el >= 0 ? users[user_el]->getUsername() : "*") + "@" + LOCALHOST;
		irc.push_cmd_queue(fd, irc.response_3(211, client->getName(), fullname, out_mess.str()));
		out_mess.str("");
	}
}

int Command::
stats_check_errors(IRC &irc, int fd)
{
	vector<Client *> &clients = irc.get_clients();
	vector<Server *> &servers = irc.get_servers();
	int server_el = IRC::find_fd(servers, fd);


	if (arguments.size() != 1 && arguments.size() != 2)
	{
		irc.push_cmd_queue(fd, irc.response_3(ERR_NEEDMOREPARAMS, "*", command, ":Syntax error"));
		return (0);
	}
	// если сообщение от сервера
	if (server_el >= 0)
	{
		if (prefix.empty())
		{
			irc.push_cmd_queue(fd, irc.response_3(ERR_NEEDMOREPARAMS, servers[server_el]->getName(), command, ":Syntax error"));
			return (0);
		}
		if (irc.find_name(clients, prefix) < 0)
		{
			irc.push_cmd_queue(fd, irc.response_3(ERR_NEEDMOREPARAMS, prefix, command, ":Syntax error"));
			return (0);
		}
	}
	return (1);
}

int Command::
cmd_stats(IRC& irc, int fd)
{
	vector<Client*>& clients					= irc.get_clients();
	vector<Server*>& servers					= irc.get_servers();
	int server_el								= IRC::find_fd(servers, fd);
	int client_el								= IRC::find_fd(clients, fd);

	if (stats_check_errors(irc, fd) != 1)
		return (0);

	// если сообщение от сервера
	if (server_el >= 0)
		client_el = irc.find_name(clients, prefix);

	if  (arguments.size() == 1 || 
		(arguments.size() == 2 && arguments[1] == irc.get_server_name()))
	{
		if (arguments[0] == "m")
			stats_m(irc, fd, clients[client_el]);
		else if (arguments[0] == "l")
			stats_l(irc, fd, clients[client_el]);
		else if (arguments[0] == "u")
			stats_u(irc, fd, clients[client_el]);
		irc.push_cmd_queue(fd, irc.response_3(RPL_ENDOFSTATS, clients[client_el]->getName(), arguments[0], ":End of STATS report"));
	}
	else if (arguments.size() == 2)
	{
		server_el = IRC::find_name(servers, arguments[1]);
		if (server_el < 0)
			irc.push_cmd_queue(fd, irc.response_3(ERR_NOSUCHSERVER, clients[client_el]->getName(), arguments[1], ":No such server"));
		else
			irc.push_cmd_queue(servers[server_el]->getSocketFd(), (prefix.empty() ? ":" + clients[client_el]->getName() + " " : "") + message + "\r\n");
	}

	return (0);
}