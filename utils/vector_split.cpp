#include "../main.hpp"

std::vector<std::string> split(std::string str, char type)
{
    std::vector<std::string> temp_vec;
    std::stringstream ss(str);
    std::string temp_str;
    while(std::getline(ss, temp_str, type))
    {
        temp_vec.push_back(temp_str);
    }
    return (temp_vec);
}