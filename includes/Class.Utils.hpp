/*
**==========================
** Class Utils - 
**==========================
*/
#pragma once

#include "main.hpp"

class Utils
{
	private:
		static bool check_network(char const *av);
		static bool check_port(char const *av);

	public:
		static bool check_av(int ac, char **av);
		static std::vector<std::string> split(std::string str, char type);
		static void exit_error(int num_error, std::string fail_msg);
		static void print_error(int num_error, std::string fail_msg);
		static void print_line(std::string msg);
		static std::string convert_int_to_str(int num);
};