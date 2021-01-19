#include "Class.Statistics.hpp"

std::string cmd_name[COMM_COUNT] = {"NICK",
									  "PASS",
									  "USER",
									  "SERVER",
									  "JOIN",
									  "OPER",
									  "QUIT",
									  "PART",
									  "NAMES",
									  "SQUIT",
									  "WHO",
									  "TOPIC",
									  "PING",
									  "PONG",
									  "MODE",
									  "KICK",
									  "PRIVMSG",
									  "AWAY",
									  "NOTICE",
									  "STATS"};

Statistics::
Statistics() :	sent_count(0),
				recv_count(0),
				sent_kbytes(0),
				recv_kbytes(0),
				start_time(utils::get_time()) {}

Statistics::
Statistics(Statistics const &src) : sent_count(src.sent_count),
									recv_count(src.recv_count),
									sent_kbytes(src.sent_kbytes),
									recv_kbytes(src.recv_kbytes),
									start_time(src.start_time)
{
	map_cmd = src.map_cmd;
}

Statistics &Statistics::
operator=(Statistics const &src)
{
	sent_count	= src.sent_count;
	recv_count	= src.recv_count;
	sent_kbytes = src.sent_kbytes;
	recv_kbytes = src.recv_kbytes;
	start_time	= src.start_time;
	map_cmd		= src.map_cmd;

	return (*this);
}

int Statistics::
getSentCount() const	{ return (this->sent_count); }

int Statistics::
getRecvCount() const	{ return (this->recv_count); }

unsigned long Statistics::
getSentKBytes() const	{ return (this->sent_kbytes); }

unsigned long Statistics::
getRecvKBytes() const	{ return (this->recv_kbytes); }

unsigned long Statistics::
getWorkingTime() const	{ return (utils::get_time() - this->start_time); }

map<string, int> &Statistics::
getMapCmd()				{ return (this->map_cmd); }


void Statistics::
sent(const string &str)
{
	for (int i = 0; i < (int)str.size(); i++)
	{
		if (str[i] == '\r')
		{
			sent_count += 1;
			sent_kbytes += str.size() * 8;
		}
	}
}

void Statistics::
recieved(string const & comm, string const &message, map<string, CmdStats>& map_cmd_stats)
{
	bool flag = false;

	for (int i = 0; i < COMM_COUNT; i++)
	{
		if (utils::is_equal(cmd_name[i], comm))
		{
			map_cmd_stats[cmd_name[i]].count++;
			fillMapCmd(cmd_name[i]);
			recv_count += 1;
			recv_kbytes += message.size() * 8;
			flag = true;
			break;
		}
	}
	if (!flag)
	{
		recv_count += 1;
		recv_kbytes += message.size() * 8;
	}
}

void Statistics::
fillMapCmd(string const &cmd_name)
{
	if (map_cmd.count(cmd_name) == 0)
		map_cmd.insert(std::make_pair<string, int>(cmd_name, 1));
	else
		map_cmd[cmd_name] = map_cmd[cmd_name] + 1;
}