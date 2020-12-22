#pragma once

#include "./main.hpp"

class Utils
{
    private:
        bool check_network(char const *av);
        bool check_port(char const *av);

    public:
        bool check_av(int ac, char const **av);
        std::vector<std::string> split(std::string str, char type);
        std::string get_random_port();
        void exit_error(int num_error, std::string fail_msg);
        void print_error(int num_error, std::string fail_msg);
        void print_line(std::string msg);
        
};