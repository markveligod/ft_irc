#ifndef BOT_HPP
#define BOT_HPP

#define FD_FREE 0
#define FD_CLIENT 1
#define FD_SERVER 2
#define WEATHER_SCRIPT "./weather.sh"

#include "main.hpp"
#include "Class.Socket.hpp"
#include "Class.Command.hpp"

using std::map;
using std::pair;
using std::queue;
using std::string;
using std::vector;

class Bot {

public:
	map<int, int>				_array_fd_select;
	queue<pair<int, string> >	_command_queue;
	fd_set						_fd_set_read;
	fd_set						_fd_set_write;
	int							_select_res;
	int							_fd_weather;
	int							_fd_file;

	void						init_fd_select();
	void						do_select();
	void						check_fd_select();
	void						push_cmd_queue(int fd, const string& str);
	int							get_response(string city);
};

#endif