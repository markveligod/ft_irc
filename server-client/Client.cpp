#include "Client.hpp"

// --------------------------------Constructors--------------------------------

Client::Client() : port(SERVER_PORT), ip(SERVER_IP) {}
Client::Client(int port, const std::string& ip, const std::string& pass) : port(port), ip(ip), pass(pass) {}


// ---------------------------Creating connection------------------------------

void	Client::create_socket()
{
	if ((this->client = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		Client::error("Esteblishing socket error");

	bzero(&this->server_addr, sizeof(this->server_addr));
	this->server_addr.sin_family = AF_INET;
	this->server_addr.sin_port = htons(this->port);
	inet_pton(AF_INET, SERVER_IP, &this->server_addr.sin_addr);

	std::cout << "Client socket created\n";
}

void	Client::connection()
{
	if ((connect(this->client, reinterpret_cast<struct sockaddr*>(&this->server_addr), sizeof(this->server_addr))) == 0)
		std::cout << "Connection to server...\n"
				  << inet_ntoa(server_addr.sin_addr)
				  << " with port number "
				  << this->port << "!!!" << std::endl;
	else
		Client::error("Connection error");
	//strcpy(_buffer, ("PASS " + this->pass + "\r\nNICK nickname\r\nUSER test test localhost 1 :Test Test\r\n").c_str());
	//strcpy(_buffer, this->pass.c_str());
	strcpy(_buffer, ("PASS " + this->pass + "\r\n").c_str());
	//_buffer[strlen(_buffer)] = '\n';
	std::cout << "Sending...\n";
	send(this->client, _buffer, BUFFER_SIZE, 0);
	/*std::cout << "Getting...\n";
	recv(this->client, _buffer, BUFFER_SIZE, 0);
	if ((std::string)_buffer == "FAIL")
	{
		std::cout << "Incorrect password\n";
		exit(EXIT_FAILURE);
	}*/
	std::cout << "Connection established!" << std::endl;
}

void Client::connection_server()
{
	if ((connect(this->client, reinterpret_cast<struct sockaddr*>(&this->server_addr), sizeof(this->server_addr))) == 0)
		std::cout << "Connection to server...\n"
				  << inet_ntoa(server_addr.sin_addr)
				  << " with port number "
				  << this->port << "!!!" << std::endl;
	else
		Client::error("Connection error");

	//recv(this->client, _buffer, BUFFER_SIZE, 0);
	
	std::cout << "Connection established!" << std::endl;
}

void	Client::chat()
{
	while (1)
	{
		std::cout << "Client: ";
		std::cin.getline(_buffer, BUFFER_SIZE - 1);
		size_t i = strlen(_buffer);
		_buffer[i] = '\r';
		_buffer[i + 1] = '\n';
		_buffer[i + 2] = '\0';
		send(this->client, _buffer, i + 2, 0);
		if (Client::end_connection(_buffer))
			break;

		/*std::cout << "Server: ";
		recv(this->client, _buffer, BUFFER_SIZE, 0);
		std::cout << _buffer << std::endl;
		if (Client::end_connection(_buffer))
			break;*/
	}
}

void	Client::send_to_server()
{

	std::cout << "Sending message to server: " << _buffer;
	send(this->client, _buffer, BUFFER_SIZE, 0);
}

void	Client::close_connectionion()
{
	close(this->client);
	std::cout << "\nEnd\n";
}

// --------------------------Helpful functions----------------------------

bool	Client::end_connection(const char* line)
{
	std::string str(line);

	if (str == "#" || str == "#\n")
		return true;
	return false;
}

void	Client::error(const std::string& str)
{
	std::cout << str << std::endl;
	exit(0);
}
