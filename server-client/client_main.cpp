#include "Client.hpp"

int main(int ac, char **av)
{
	if (ac != 2)
	{
		std::cout << "./client <port>\n";
		exit(EXIT_FAILURE);
	}
	Client client = Client(atoi(av[1]), SERVER_IP);

	client.create_socket();
	client.connection();
	while (1)
		client.chat();
	client.close_connection();

	return (0);
}