#pragma once

/*
**==========================
**         Colors
**==========================
*/

#define RED "\033[1;31m"
#define GREEN "\033[1;32m"
#define YELLOW "\033[1;33m"
#define CYAN "\033[1;36m"
#define RESET "\033[0m"

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
#include <cstdlib>
#include <netinet/in.h> //struct sockaddr_in
#include <sys/socket.h> //socket

#include "ClassServer.hpp"

/*
**==========================
**       Error's
**==========================
*/

#define ERR_COUNT 11
#define ERR_CHECKAV 12
#define ERR_FDSERVER 13

/*
**==========================
**       Configurate
**==========================
*/

#define DEF_HOST "127.0.0.1"
//#define DEF_PORT "7777"
#define DEF_PASS "123456789"

/*
**==========================
**       Prototype
**==========================
*/

bool check_av(int ac, char const **av);
std::vector<std::string> split(std::string str, char type);
std::string get_random_port();

//print_line.cpp
void exit_error(int num_error, std::string fail_msg);
void print_error(int num_error, std::string fail_msg);
void print_line(std::string msg);
