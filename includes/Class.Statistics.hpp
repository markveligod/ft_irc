#pragma once

#include <set>
#include "main.hpp"

using std::map;
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
		int					sent_count;
		int					recv_count;
		unsigned long		sent_kbytes;
		unsigned long 		recv_kbytes;
		unsigned long		start_time;
		map<string, int>	map_cmd;

	public:
		Statistics();
		Statistics(Statistics const &src);
		Statistics &operator=(Statistics const &src);

		void				recieved(string const & comm, string const &message, map<string, CmdStats>& map_cmd_stats);
		void				sent(const string &str);

		// void				inc_sent(int count, int kbytes);
		// void				inc_recv(int count, int kbytes);
		void				fillMapCmd(string const &cmd_name);

		int					getSentCount() const;
		int					getRecvCount() const;
		unsigned long		getSentKBytes() const;
		unsigned long		getRecvKBytes() const;
		unsigned long		getWorkingTime() const;
		map<string, int>&	getMapCmd();
};
