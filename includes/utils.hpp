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
	void print_message(int fd, std::vector<std::string> &buffer_cmd);
	void print_client(int n, std::string msg);
	void print_command(std::string msg);
	std::string int_to_str(int num);
}