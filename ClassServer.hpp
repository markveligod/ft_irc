#pragma once
#include "./main.hpp"
#include "./AServer.hpp"

class Server: public AServer
{
    private:
        int FdServer;

    public:
        Server();
        Server(const Server& other);
        ~Server();

        Server &operator=(const Server& other);
};