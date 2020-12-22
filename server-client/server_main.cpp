#include "Server.hpp"
#include "Client.hpp"

int main()
{
	Server server = Server();
	Client client = Client(1080, SERVER_IP);

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