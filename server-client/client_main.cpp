#include "Client.hpp"

int main(int ac, char **av)
{
	if (ac != 3)
	{
		std::cout << "./client <port> <pass>\n";
		exit(EXIT_FAILURE);
	}
	Client client = Client(atoi(av[1]), SERVER_IP, av[2]);

	client.create_socket();
	client.connection();
	while (1)
		client.chat();
	client.close_connectionion();

	return 0;
}