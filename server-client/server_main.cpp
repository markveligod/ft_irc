#include "IRChpp"
#include "Client.hpp"

int main(int ac, char **av)
{
	IRC server = IRC();
	Client client = Client(1080, SERVER_IP, av[2]);

	server.create_socket();
	server.connection();

	client.create_socket();
	client.connection_server();

	while (1)
	{
		server.chat();
		client.set_buffer(server.get_buffer().c_str());
		client.send_to_server();
	}

	return (0);
}