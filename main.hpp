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

/*
**==========================
**       Error's
**==========================
*/

#define ERR_COUNT 11
#define ERR_CHECKAV 12

/*
**==========================
**       Prototype
**==========================
*/

bool check_av(int ac, char const **av);