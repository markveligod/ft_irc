#include "AServer.hpp"

AServer::AServer(int port) : port(port) {}

bool AServer::end_connection()
{
	std::string str(this->buffer);

	if (str == "#" || str == "#\n")
		return (true);
	return (false);
}