#include "../main.hpp"

std::string get_random_port()
{
    std::string temp;
    const char *local = "123456789";
    const char *first_local = "123456";

    temp.push_back(first_local[rand() % 6]);
    for (size_t i = 0; i < 3; i++)
    {
        temp.push_back(local[rand() % 9]);
    }
    return (temp);
}