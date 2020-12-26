#ifndef CLASS_USER_HPP
# define CLASS_USER_HPP

#include "main.hpp"
#include "Class.Client.hpp"

struct ModeUser
{
	bool i;
	bool s;
	bool w;
	bool o;
};

class User : public Client
{
private:
	std::string 	username;
	std::string 	hostname;
	std::string 	servername;
	std::string 	realname;
	struct ModeUser	mode;

public:
	User(Client const &src);

	void			user_from_client(std::string username, std::string hostname, 
									 std::string servername, std::string realname);
	void			user_from_client(std::string username, std::string hostname,
									 std::string servername, std::string realname, ModeUser mode);
};

#endif