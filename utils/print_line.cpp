#include "../main.hpp"

void print_line(std::string msg)
{
    std::cout << CYAN << "[SERVER]: " << YELLOW << msg << std::endl << RESET;
}

void print_error(int num_error, std::string fail_msg)
{
    std::cerr << CYAN << "[SERVER]: " << RED << "ERROR: " << RESET << "code: " << num_error << YELLOW << fail_msg << std::endl << RESET;
}

void exit_error(int num_error, std::string fail_msg)
{
    print_error(num_error, fail_msg);
    exit(num_error);
}