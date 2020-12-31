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
	Client *		client;
	struct ModeUser	mode;

public:
	//User(Client const &src);
	User(Client * src);

	void			user_from_client(std::string username, std::string hostname, 
									 std::string servername, std::string realname);
	void			user_from_client(std::string username, std::string hostname,
									 std::string servername, std::string realname, ModeUser mode);

	void			change_user(std::string const & username, std::string const & hostname,
								std::string const & servername, std::string const realname);

	void			setUsername(std::string const & username);
	void			setHostname(std::string const & hostname);
	void			setServername(std::string const & servername);
	void			setRealname(std::string const & realname);
	void			setMode(ModeUser mode);
	void			print_user();

	std::string const &		getUsername() const;
	std::string const &		getHostname() const;
	std::string const &		getServername() const;
	std::string const &		getRealname() const;
};

#endif