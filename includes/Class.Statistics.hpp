#pragma once

#include <set>
#include "main.hpp"

using std::map;
using std::pair;
using std::string;

struct CmdStats
{
 	string				cmd_name;
 	int					count;
 	int					byte_count;

 	CmdStats();
 	CmdStats(string cmd_name);
};

class Statistics
{
	private:
		int										sent_count;
		int										recv_count;
		unsigned long							sent_kbytes;
		unsigned long 							recv_kbytes;
		time_t									time_start;
		int										queue_count;
		map<string, pair<int, unsigned long> >	map_cmd;		// cmd_name - count - bytes

	public:
		Statistics();
		Statistics(Statistics const &src);
		Statistics &operator=(Statistics const &src);

		void				recieved(string const & comm, string const &message);
		void				sent(const string &str);
		void				queued(const string &str, bool add);

		void				fillMapCmd(string const &cmd_name, unsigned long bytes);

		int					getSentCount() const;
		int					getRecvCount() const;
		int					getQueueCount() const;
		unsigned long		getSentKBytes() const;
		unsigned long		getRecvKBytes() const;
		unsigned long		getWorkingTime() const;
		struct tm * 		getWorkingTime_t() const;
		string const 		getStartDate() const;
		map<string, pair<int, unsigned long> >& getMapCmd();
};
