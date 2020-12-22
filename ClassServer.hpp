#pragma once
#include "./main.hpp"

class Server
{
    private:
        int FdServer;

    public:
        Server();
        Server(const Server& other);
        ~Server();

        Server &operator=(const Server& other);
};