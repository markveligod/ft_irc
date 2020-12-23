#ifndef CLASSCLIENT_HPP
# define CLASSCLIENT_HPP

# include "./AServer.hpp"

class Client : public AServer
{
private:
	std::string const	ip;
	std::string pass;

public:
	Client(int port, std::string const & ip, std::string const &pass);

	void				create_socket();
	void				connection();
	void				send_message();
	void				set_buffer(char const *buffer);
};

#endif