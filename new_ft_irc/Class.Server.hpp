/*
**==========================
** Class Server - 
**==========================
*/

#include "./main.hpp"
#include "./Class.Socket.hpp"
//#include "./Class.Getting.hpp"
//#include "./Class.Sending.hpp"

class Server: public Socket
{
    private:
        std::string ip_network;
        int port_network;
        std::string pass_network;
        int port_curr;
        std::string pass_curr;
        Socket server_locale;
        Socket server_network;

    public:
        Server();
        Server(std::string ip_network, int port_network, std::string pass_network, int port_curr, std::string pass_curr);

        Server &operator=(const Server &other);

        void create_socket_network();
        void create_socket_locale();

        void bind_socket_network();
        void bind_socket_locale();
};