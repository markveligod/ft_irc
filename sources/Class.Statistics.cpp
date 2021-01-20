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
				time_start(time(NULL)),
				queue_count(0) {}

Statistics::
Statistics(Statistics const &src) : sent_count(src.sent_count),
									recv_count(src.recv_count),
									sent_kbytes(src.sent_kbytes),
									recv_kbytes(src.recv_kbytes),
									queue_count(src.queue_count)
{
	time_start	= src.time_start;
	map_cmd		= src.map_cmd;
}

Statistics &Statistics::
operator=(Statistics const &src)
{
	sent_count	= src.sent_count;
	recv_count	= src.recv_count;
	sent_kbytes = src.sent_kbytes;
	recv_kbytes = src.recv_kbytes;
	time_start	= src.time_start;
	queue_count = src.queue_count;
	map_cmd		= src.map_cmd;

	return (*this);
}

int Statistics::
getSentCount() const	{ return (this->sent_count); }

int Statistics::
getRecvCount() const	{ return (this->recv_count); }

int Statistics::
getQueueCount() const	{ return (this->queue_count); }

unsigned long Statistics::
getSentKBytes() const	{ return (this->sent_kbytes); }

unsigned long Statistics::
getRecvKBytes() const	{ return (this->recv_kbytes); }

unsigned long Statistics::
getWorkingTime() const	
{
	time_t curr_time = time(NULL);
	double diff = difftime(curr_time, time_start);

	return ((unsigned long)diff);
}

map<string, pair<int, unsigned long> > &Statistics::
getMapCmd()				{ return (this->map_cmd); }

struct tm * Statistics::
getWorkingTime_t() const {
	time_t curr_time = time(NULL);
	const time_t diff = difftime(curr_time, time_start);
	struct tm *timeinfo;

	timeinfo = localtime(&diff);
	return (timeinfo);
}

void Statistics::
queued(const string &str, bool add)
{
	for (int i = 0; i < (int)str.size(); i++)
	{
		if (str[i] == '\r')
			queue_count += (add ? 1 : -1);
	}
}

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
recieved(string const & comm, string const &message)
{
	bool flag = false;

	for (int i = 0; i < COMM_COUNT; i++)
	{
		if (utils::is_equal(cmd_name[i], comm))
		{
			fillMapCmd(cmd_name[i], message.size() * 8);
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
fillMapCmd(string const &cmd_name, unsigned long bytes)
{
	if (map_cmd.count(cmd_name) == 0)
		map_cmd.insert(std::make_pair<string, pair<int, unsigned long> >(cmd_name, std::make_pair<int, unsigned long>(1, bytes)));
	else
	{
		map_cmd[cmd_name].first += 1;
		map_cmd[cmd_name].second += bytes;
	}
}
