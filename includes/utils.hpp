/*
**==========================
** Class utils - 
**==========================
*/
#pragma once

#include "main.hpp"

namespace utils
{
	struct less
	{
		bool
		operator()(std::string x, std::string y) const
		{
			std::transform(x.begin(), x.end(), x.begin(), tolower);
			std::transform(y.begin(), y.end(), y.begin(), tolower);
			return x < y;
		}
	};

	bool check_av(int ac, char **av);
	std::vector<std::string> split(std::string str, char type);
	void exit_error(int num_error, std::string fail_msg);
	void print_error(int num_error, std::string fail_msg);
	void print_line(std::string msg);
	void print_message(int fd, std::vector<std::string> &buffer_cmd);
	void print_client(int n, std::string msg);
	void print_command(int fd, std::string msg);
	std::string int_to_str(int num);
	bool is_equal(std::string s1, std::string s2);
}