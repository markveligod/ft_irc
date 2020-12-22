#ifndef CLASSCLIENT_HPP
# define CLASSCLIENT_HPP

# pragma once
# include "./AServer.hpp"

class Client : public AServer
{
private:
	std::string const	ip;

public:
	Client(int port, std::string const & ip);

	void				create_socket();
	void				connection();
	void				send_message();
	void				set_buffer(char const *buffer);
};

#endif