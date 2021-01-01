/*
**==========================
** Class utils - 
**==========================
*/
#pragma once

#include "main.hpp"

namespace utils
{
	bool check_av(int ac, char **av);
	std::vector<std::string> split(std::string str, char type);
	void exit_error(int num_error, std::string fail_msg);
	void print_error(int num_error, std::string fail_msg);
	void print_line(std::string msg);
	std::string convert_int_to_str(int num);
}