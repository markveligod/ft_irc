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
Statistics() :	_sent_count(0),
				_recv_count(0),
				_sent_kbytes(0),
				_recv_kbytes(0),
				_time_start(time(NULL)),
				_queue_count(0) {}

Statistics::
Statistics(Statistics const &src) : _sent_count(src._sent_count),
									_recv_count(src._recv_count),
									_sent_kbytes(src._sent_kbytes),
									_recv_kbytes(src._recv_kbytes),
									_queue_count(src._queue_count)
{
	_time_start	= src._time_start;
	_map_cmd		= src._map_cmd;
}

Statistics &Statistics::
operator=(Statistics const &src)
{
	_sent_count	= src._sent_count;
	_recv_count	= src._recv_count;
	_sent_kbytes = src._sent_kbytes;
	_recv_kbytes = src._recv_kbytes;
	_time_start	= src._time_start;
	_queue_count = src._queue_count;
	_map_cmd		= src._map_cmd;

	return (*this);
}

int Statistics::
getSentCount() const	{ return (_sent_count); }

int Statistics::
getRecvCount() const	{ return (_recv_count); }

int Statistics::
getQueueCount() const	{ return (_queue_count); }

unsigned long Statistics::
getSentKBytes() const	{ return (_sent_kbytes); }

unsigned long Statistics::
getRecvKBytes() const	{ return (_recv_kbytes); }

unsigned long Statistics::
getWorkingTime() const	
{
	time_t curr_time = time(NULL);
	double diff = difftime(curr_time, _time_start);

	return ((unsigned long)diff);
}

map<string, pair<int, unsigned long> > &Statistics::
getMapCmd()				{ return (_map_cmd); }

struct tm * Statistics::
getWorkingTime_t() const {
	time_t curr_time = time(NULL);
	const time_t diff = difftime(curr_time, _time_start);
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
			_queue_count += (add ? 1 : -1);
	}
}

void Statistics::
sent(const string &str)
{
	for (int i = 0; i < (int)str.size(); i++)
	{
		if (str[i] == '\r')
		{
			_sent_count += 1;
			_sent_kbytes += str.size() * 8;
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
			_recv_count += 1;
			_recv_kbytes += message.size() * 8;
			flag = true;
			break;
		}
	}
	if (!flag)
	{
		_recv_count += 1;
		_recv_kbytes += message.size() * 8;
	}
}

void Statistics::
fillMapCmd(string const &cmd_name, unsigned long bytes)
{
	if (_map_cmd.count(cmd_name) == 0)
		_map_cmd.insert(std::make_pair<string, pair<int, unsigned long> >(cmd_name, std::make_pair<int, unsigned long>(1, bytes)));
	else
	{
		_map_cmd[cmd_name].first += 1;
		_map_cmd[cmd_name].second += bytes;
	}
}
