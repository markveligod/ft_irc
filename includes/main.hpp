#pragma once

/*
**==========================
**         Colors
**==========================
*/

#define RED "\033[0;31m"
#define GREEN "\033[0;32m"
#define YELLOW "\033[0;33m"
#define BLUE "\033[0;34m"
#define PURPLE "\033[0;35m"
#define CYAN "\033[0;36m"
#define RESET "\033[0m"

/*
**==========================
**       Include's
**==========================
*/

#include <algorithm>
#include <limits>
#include <iostream>
#include <iomanip>
#include <vector>
#include <queue>
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
#include <csignal>
#include <ctime>

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
#define ERR_SELECT 19
#define ERR_PASSWORD 20
#define ERR_NICKNAME 21
#define ERR_DATACLIENT 22
#define ERR_FINDFD 23
#define ERR_ARG_NUMBER 24

/*
**==========================
**       Configurate
**==========================
*/

#define SERVERNAME "ft_irc.net"
#define LOCALHOST "127.0.0.1"
#define DEF_PORT "0"
#define BUFFER_SIZE	512
#define DEF_PASS "123"
#define FD_REQUEST 1
#define FD_COMMIT 2
#define MAX_MESSAGE_LEN 510

/*
**==========================
**       Class
**==========================
*/

#include "utils.hpp"
#include "ctime"
//#include "Class.Client.hpp"
//#include "Class.Message.hpp"
//#include "Class.User.hpp"
#include "codes.hpp"

#define COMM_COUNT 23

#define ADMINME			" :Ум и красота - leweather"
#define ADMINLOC1		" :Веселый озорник - ckakuna"
#define ADMINLOC2		" :Еще есть - pdemocri"
#define ADMINEMAIL		" :Email-a у нас нет:("

