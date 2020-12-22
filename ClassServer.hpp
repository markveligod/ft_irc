#pragma once
#include "./main.hpp"

class Server
{
    private:
        int FdServer;
        struct sockaddr_in server_address;

    public:
        Server();
        Server(const Server& other);
        ~Server();

        Server &operator=(const Server& other);
};