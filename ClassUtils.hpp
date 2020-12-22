#pragma once

#include "./main.hpp"

class Utils
{
    private:
        static bool check_network(char const *av);
        static bool check_port(char const *av);

    public:
        static bool check_av(int ac, char const **av);
        static std::vector<std::string> split(std::string str, char type);
        static std::string get_random_port();
        static void exit_error(int num_error, std::string fail_msg);
        static void print_error(int num_error, std::string fail_msg);
        static void print_line(std::string msg);
        
};