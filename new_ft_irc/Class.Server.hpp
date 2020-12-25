/*
**==========================
** Class Server - 
**==========================
*/

#include "./main.hpp"
#include "./Class.Socket.hpp"
//#include "./Class.Getting.hpp"
//#include "./Class.Sending.hpp"

class Server
{
    private:
        std::string         ip_network;
        int                 port_network;
        std::string         pass_network;
        int                 port_curr;
        std::string         pass_curr;
        Socket              server_locale;
        Socket              server_network;
        // std::vector<int>    array_fd_select;
		std::map<int, int>	array_fd_select;
        fd_set              fd_set_sockets;
        int                 select_res;

    public:
        Server();
        Server(std::string ip_network,
			   std::string port_network,
			   std::string pass_network,
			   std::string port_curr,
			   std::string pass_curr);

        Server &operator=(const Server &other);

        void create_socket_network();
        void create_socket_locale();

        void init_fd_select();
        void do_select();
        void check_fd_select();
};