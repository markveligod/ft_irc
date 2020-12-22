#include "main.hpp"
#include "ClassClient.hpp"
#include "ClassServer.hpp"

int main(int ac, char const **av)
{
	std::vector<std::string> network;
	srand(time(NULL));
    
    if (ac != 3 && ac != 4)
        Utils::exit_error(ERR_COUNT, "ARG: ./ircserv [host:port_network:password_network] <port> <password>");
    if (!Utils::check_av(ac, av))
        Utils::exit_error(ERR_CHECKAV, "Incorrect enter to argv!");

    if (ac == 4)
        network = Utils::split(av[1], ':');
    else
    {
        network.push_back(DEF_HOST);
        network.push_back(Utils::get_random_port());
        network.push_back(DEF_PASS);
    }
 
    //test cout 
    // std::cout << network[0] << std::endl;
    // std::cout << network[1] << std::endl;
    // std::cout << network[2] << std::endl;

	//test connection with client and server
	Server server_part = Server(ac == 4 ? av[2] : av[1]);
	//Server server_part2 = Server(ac == 4 ? av[2] : av[1]);
	Client client_part = Client(atoi(network[1].c_str()), network[0]);

	server_part.create_socket();
	server_part.connection();
	

	client_part.create_socket();
	client_part.connection();
	server_part.socket_listen();

	while (1)
	{
		server_part.init_fd_select();
		server_part.do_select();
		server_part.check_fd_select(&client_part);
	}

/*	while (1)
	{
		
		server_part.socket_accept();
		while (server_part.check_fd_server())
		{
			//server_part.send_message();
			server_part.recv_message();
			client_part.set_buffer(server_part.get_buffer().c_str());
			client_part.send_message();
		}
	}
*/
	return (0);
}