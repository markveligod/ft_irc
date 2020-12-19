#include "Server.hpp"

int main()
{
	Server server = Server();

	server.create_socket();
	server.connection();
	server.chat();

	return (0);
}