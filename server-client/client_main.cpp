#include "Client.hpp"

int main()
{
	Client client = Client();

	client.create_socket();
	client.connection();
	while (1)
		client.chat();
	client.close_connection();

	return (0);
}