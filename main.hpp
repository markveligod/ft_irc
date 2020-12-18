#pragma once

/*
**==========================
**       Include's
**==========================
*/

#include <iostream>
#include <vector>
#include <cstring>
#include <cctype>
#include <sstream>

/*
**==========================
**       Error's
**==========================
*/

#define ERR_COUNT 11
#define ERR_CHECKAV 12

/*
**==========================
**       Configurate
**==========================
*/

#define DEF_HOST "127.0.0.1"
#define DEF_PORT "7777"
#define DEF_PASS "123456789"

/*
**==========================
**       Prototype
**==========================
*/

bool check_av(int ac, char const **av);
std::vector<std::string> split(std::string str, char type);
