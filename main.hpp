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
#include <map>
#include <cstring> // for bzero and strcpy in Linux
#include <cctype>
#include <sstream>
#include <cstdlib>
#include <netinet/in.h> //struct sockaddr_in
#include <sys/socket.h> //socket
#include <string>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <netdb.h>

/*
**==========================
**       Error's
**==========================
*/

#define ERR_COUNT 11
#define ERR_CHECKAV 12
#define ERR_FDSERVER 13
#define ERR_SOCKET 14
#define ERR_BIND 15
#define ERR_ACCEPT 16
#define ERR_LISTEN 17
#define ERR_CONNECT_TO_SERVER 18

/*
**==========================
**       Configurate
**==========================
*/

#define DEF_HOST "127.0.0.1"
//#define DEF_PORT "7777"
#define BUFFER_SIZE	1024
#define DEF_PASS "123456789"

/*
**==========================
**       Class
**==========================
*/

// #include "AServer.hpp"
#include "Class.Utils.hpp"
// #include "ClassServer.hpp"
// #include "ClassClient.hpp"
